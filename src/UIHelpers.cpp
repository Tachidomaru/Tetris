#include "UIHelpers.hpp"

int getPreviewXOffset(TetrominoType type)
{
    switch(type)
    {
        case TetrominoType::I:
        case TetrominoType::O:
            return -78;
        default:
            return -63;
    }
}