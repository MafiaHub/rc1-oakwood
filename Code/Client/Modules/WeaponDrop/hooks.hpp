inline auto init() {
	MemoryPatcher::InstallJmpHook(0x0058D4C6, 0x0058D553); // Remove dropped clip
    MemoryPatcher::InstallJmpHook(0x00585D90, 0x00585DCB); // Disable local player weapon drop
    MemoryPatcher::InstallJmpHook(0x0057FAA0, 0x00580196); // Disable weapon drops
}