#include <chrono>
// #include <cstdint>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>
// #include <glm/vec3.hpp>
// #include <glm/vec4.hpp>
// #include <immintrin.h>
// #include <vector>

#include <Consts.h>
#include <Utils.h>
#include <World.h>

#include <exception>
#include <iostream>

int main() {
    // naive initial version

    World* world = new World;

    world->systemManager.initSystems(world->windowManager.getWindow());

    Utils::seedRNG();

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    do {
        std::uint32_t id = world->entityManager.spawn(AGENT);
        if (id != UINT32_MAX) {
            world->componentManager.addComponents(AGENT);
        }
    } while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5));

    do {
        world->systemManager.update(&world->windowManager, &world->componentManager, &world->propertiesManager);
    } while (std::chrono::steady_clock::now() - start < std::chrono::seconds(300));
    // while windowManager.shouldClose()

    // start = std::chrono::steady_clock::now();

    // std::uint32_t id = MAX_ENTITIES - 1;
    // do {
    //     std::uint32_t index = world->entityManager.destroy(id, AGENT);
    //     std::cout << "destroyed id: " << id << std::endl;
    //     --id;

    //     if (index != UINT32_MAX) {
    //         world->componentManager.removeComponents(index, AGENT);
    //     }
    // } while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5) && id < MAX_ENTITIES);

    delete world;

    // run
    return EXIT_SUCCESS;
}
