#pragma once

namespace yunikEngine {
    class Scene {
        public:
        virtual ~Scene (void) {}
        virtual Scene* update (void) = 0;
    };
}
