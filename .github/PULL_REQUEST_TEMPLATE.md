**Please check if your PR fulfills these requirements**

- The title and the commit message(s) are descriptive
- Small commits made to fix your PR have been squashed to avoid history pollution
- Tests have been added for new features or bug fixes
- API of new functions and classes are documented
- If you PR introduces backward incompatible changes, update the version number
in include/xtl/xtl_config.hpp according to the following rules:
    - if XTL_VERSION_PATCH is already 0-dev, you have nothing to do
    - otherwise, set XTL_VERSION_PATCH to 0-dev and increase XTL_VERSION_MINOR by 1
