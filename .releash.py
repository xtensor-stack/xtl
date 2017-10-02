from releash import *

gitpush = ReleaseTargetGitPush('upstream', 'master')

xtl = add_package(path=".", name="xtl")
version_xtl = VersionSourceAndTargetHpp(xtl, '{path}/include/xtl/xtl_config.hpp', prefix='XTL_VERSION_')
gittag_xtl = ReleaseTargetGitTagVersion(version_source=version_xtl, prefix='', annotate=True)

xtl.version_source = version_xtl
xtl.version_targets.append(version_xtl)

xtl.release_targets.append(gittag_xtl)
xtl.release_targets.append(gitpush)

source_tarball_filename = 'https://github.com/QuantStack/xtl/archive/{version}.tar.gz'.format(version=version_xtl)
xtl.release_targets.append(ReleaseTargetCondaForge(xtl, '../xtl-feedstock', source_tarball_filename=source_tarball_filename))

