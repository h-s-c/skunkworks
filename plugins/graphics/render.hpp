// Public Domain

#include "plugins/graphics/sprite.hpp"

class Render
{
  public:
    Render();
    void Draw(double deltatime);
    
  private:
    std::vector<Sprite> sprites;
};
