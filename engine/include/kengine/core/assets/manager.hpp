#ifndef KENGINE_CORE_ASSETS_MANAGER_HPP
#define KENGINE_CORE_ASSETS_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>

#include <kengine/types.hpp>
#include <kengine/singleton.hpp>
#include <kengine/core/assets/asset.hpp>

namespace kengine::core::assets {

class Manager : public kengine::Singleton<Manager> {
public:

};

} // namespace kengine::core::assets

#endif