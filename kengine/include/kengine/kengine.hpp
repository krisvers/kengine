#ifndef KRISVERS_KENGINE_HPP
#define KRISVERS_KENGINE_HPP

#include <cstdint>
#include <cstddef>
#include <kengine/graphics/renderer.hpp>

namespace kengine {

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef size_t usize;
typedef float f32;
typedef double f64;

class KEngine {
private:
	graphics::KRenderer* m_renderer;

public:
	KEngine();
	KEngine(graphics::KRenderer* renderer);

	int main();
};

} // namespace kengine

#endif
