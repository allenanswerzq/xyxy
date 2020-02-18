load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def github_repository(*, name=None, remote=None, commit=None, tag=None,
            branch=None, build_file=None, build_file_content=None,
            sha256=None, archive_suffix=".zip", shallow_since=None,
            strip_prefix=True, url=None, path=None, **kwargs):
  """
  Conveniently chooses between archive, git, etc. GitHub repositories.
  Prefer archives, as they're smaller and faster due to the lack of history.

  One of {commit, tag, branch} must also be provided (as usual).

  sha256 should be omitted (or None) when the archive hash is unknown, then
  updated ASAP to allow caching & avoid repeated downloads on every build.

  If remote       == None , it is an error.
  If name         == None , it is auto-deduced, but this is NOT recommended.
  If build_file   == True , it is auto-deduced.
  If strip_prefix == True , it is auto-deduced.
  If url          == None , it is auto-deduced.
  If sha256       != False, uses archive download (recommended; fast).
  If sha256       == False, uses git clone (NOT recommended; slow).
  If path         != None , local repository is assumed at the given path.
  """
  GIT_SUFFIX = ".git"

  treeish = commit or tag or branch
  if not treeish: fail("Missing commit, tag, or branch argument")
  if remote == None: fail("Missing remote argument")

  if remote.endswith(GIT_SUFFIX):
    remote_no_suffix = remote[:len(remote) - len(GIT_SUFFIX)]
  else:
    remote_no_suffix = remote
  project = remote_no_suffix.split("//", 1)[1].split("/")[2]

  if name == None:
    name = project.replace("-", "_")
  if strip_prefix == True:
    strip_prefix = "%s-%s" % (project, treeish)
  if url == None:
    url = "%s/archive/%s%s" % (remote_no_suffix, treeish, archive_suffix)
  if build_file == True:
    build_file = "@//%s:%s" % ("bazel", "BUILD." + name)

  if path != None:
    if build_file or build_file_content:
      native.new_local_repository(name=name, path=path,
                    build_file=build_file,
                    build_file_content=build_file_content,
                    **kwargs)
    else:
      native.local_repository(name=name, path=path, **kwargs)
  elif sha256 == False:
    if build_file or build_file_content:
      new_git_repository(name=name, remote=remote, build_file=build_file,
                 commit=commit, tag=tag, branch=branch,
                 shallow_since=shallow_since,
                 build_file_content=build_file_content,
                 strip_prefix=strip_prefix, **kwargs)
    else:
      git_repository(name=name, remote=remote, strip_prefix=strip_prefix,
               commit=commit, tag=tag, branch=branch,
               shallow_since=shallow_since, **kwargs)
  else:
    http_archive(name=name, url=url, sha256=sha256, build_file=build_file,
           strip_prefix=strip_prefix,
           build_file_content=build_file_content, **kwargs)

def qian_deps_setup():
  github_repository(
    name = "bazel_common",
    commit = "f1115e0f777f08c3cdb115526c4e663005bec69b",
    remote = "https://github.com/google/bazel-common",
    sha256 = "1e05a4791cc3470d3ecf7edb556f796b1d340359f1c4d293f175d4d0946cf84c",
  )

  github_repository(
    name = "com_google_googletest",
    commit = "3306848f697568aacf4bcca330f6bdd5ce671899",
    remote = "https://github.com/google/googletest",
    sha256 = "2625a1d301cd658514e297002170c2fc83a87beb0f495f943601df17d966511d",
  )

  github_repository(
    name = "com_github_gflags_gflags",
    commit = "e171aa2d15ed9eb17054558e0b3a6a413bb01067",
    remote = "https://github.com/gflags/gflags",
    sha256 = "da72f0dce8e3422d0ab2fea8d03a63a64227b0376b3558fd9762e88de73b780b",
  )

  github_repository(
    name = "com_github_google_glog",
    commit = "925858d9969d8ee22aabc3635af00a37891f4e25",
    remote = "https://github.com/google/glog",
    sha256 = "dbe787f2a7cf1146f748a191c99ae85d6b931dd3ebdcc76aa7ccae3699149c67",
    patches = [
      "//thirdparty/patches:glog-stack-trace.patch",
    ],
  )

