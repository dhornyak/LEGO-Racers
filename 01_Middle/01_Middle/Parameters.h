#pragma once

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#endif // M_PI

enum class CubeHeight { THIN = 1, NORMAL = 3 };

enum class CubeColor { RED, GREEN, BLUE, YELLOW, GRAY, WHITE, BLACK, BROWN, PEACH };

enum class Scene { EDITING = 1, RACING = 2, FINISH = 3 };

enum class LightOptions { EDITING = 1, RACING_DAY = 2, RACING_NIGHT = 3, FINISH = 4 };
