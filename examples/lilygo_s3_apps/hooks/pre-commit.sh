#!/bin/bash

version_regex='v([0-9]+)\.([0-9]+)\.?([0-9]*)-?([a-z|A-Z|0-9|_]+)-([0-9]+)-g([0-9|a-z]+)'
git_string=$(git describe --tags --long)

if [[ $git_string =~ $version_regex ]]; then
	major_version="${BASH_REMATCH[1]}"
	minor_version="${BASH_REMATCH[2]}"
	patch_version="${BASH_REMATCH[3]}"
	app_name="${BASH_REMATCH[4]}"
	commits_ahead="${BASH_REMATCH[5]}"
else
	echo "Error: git describe did not output a valid version string. Unable to update git_version.h" >&2
	exit 1
fi

version_num="${major_version}.${minor_version}.${patch_version}"

# Working directory of a git hook is always the root of the repo
cat > $(pwd)/main/git_version.h <<EOM
#define MAJOR_VERSION            $major_version
#define MINOR_VERSION            $minor_version
#define PATCH_VERSION            $patch_version
#define COMMITS_AHEAD_OF_VERSION $commits_ahead
#define VERSION_STRING           "v${version_num}"
#define APP_NAME_STRING          "${app_name}"
EOM

git add $(pwd)/main/git_version.h
