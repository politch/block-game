#include "webgpu.h"

#include <ostream>

#if defined(__EMSCRIPTEN__)
std::ostream &operator<<(std::ostream &os, wgpu::ErrorType error)
{
	switch (error) {
	case wgpu::ErrorType::NoError:
		os << "NoError";
		break;
	case wgpu::ErrorType::Internal:
		os << "Internal";
		break;
	case wgpu::ErrorType::OutOfMemory:
		os << "OutOfMemory";
		break;
	case wgpu::ErrorType::Validation:
		os << "Validation";
		break;
	case wgpu::ErrorType::Unknown:
		os << "Unknown";
		break;
	}

	return os;
}

std::ostream &operator<<(std::ostream &os, wgpu::StringView view)
{
	os.write(view.data, view.length);
	return os;
}
#endif
