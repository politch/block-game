#include "config.h"

Config &Config::Get()
{
	static Config config;
	return config;
}
