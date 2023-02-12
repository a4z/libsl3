# Reveal sl3

[libsl3](https://github.com/a4z/libsl3) is  project with a 15+ years history.
At some point in time it has been frozen in time since it had users on RHEL6 and RHEL7 who relied on it.

The library is still useful, but needs some updates to be usable on todays infrastructure.

The next goal is to modernize libsl3 to a state so it can be used again.

## First iteration

The first iteration includes:

* Change from CMake 2.8.11 to 3.20.+
* Clean up CMake usage as far as possible
* Remove internal sqlite and doctest, use Conan
* Targets must build/test again with the updated infrastructure
* Remove travis/appveyor/azure CI history and switch to github actions
* Use new github workflow to publish the API documentation

Once this is done, further modernization, and also improvements, will be possible.

### Narrow scope

Remove the static / dynamic lib build option. This was only useful for RPM generation, but shall be handled different in future.

Focus on static library usage, make dynamic builds an optional goal for iteration 1.

### Don't forget

As long as pinned to Conan V1, mention

and the usual other setup things in the docs/readme.

* use a custom `CONAN_USER_HOME`
* `conan config set general.revisions_enabled=1` (or ensure `CONAN_REVISIONS_ENABLED=1`, best having both)
* `conan profile new --detect default`
* `conan config set storage.download_cache=$HOME/Downloads/conan_cache`

### Notes

Interesting: doctest 2.4.9 makes unit test fail when CHECKs are in a loop

## Second iteration

TBD.
