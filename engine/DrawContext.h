#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <cstdint>

#include "MathUtil.h"
#include <SFML/Graphics.hpp>

namespace CMPUT350 {

struct RGBColor {
    uint8_t r, g, b;

    /** Creates an 8-bit RGB color.
     * @param r Red channel from 0 to 255.
     * @param g Green channel from 0 to 255.
     * @param b Blue channel from 0 to 255.
     */
    RGBColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

namespace Colors {

const RGBColor red(255, 0, 0);
const RGBColor green(0, 255, 0);
const RGBColor blue(0, 0, 255);
const RGBColor yellow(255, 255, 0);
const RGBColor cyan(0, 255, 255);
const RGBColor magenta(255, 0, 255);
const RGBColor white(255, 255, 255);
const RGBColor black(0, 0, 0);
const RGBColor gray(100, 100, 100);
const RGBColor grey(200, 200, 200);

}  // namespace Colors

class DrawContext {
public:
    /** Creates a drawing interface for a window and its font resource.
     * @param mWindow Window that receives draw calls.
     * @param font Font used for text drawing; it must remain valid while this context is used.
     */
    DrawContext(std::shared_ptr<sf::RenderWindow> mWindow, std::shared_ptr<sf::Font> font);

    /** Draws text with its top-left origin at a pixel position.
     * @param text Text to display.
     * @param pixelSize Character size in pixels.
     * @param p Top-left position in window coordinates.
     * @param c RGB fill color.
     * @return No value.
     */
    void DrawText(const std::string& text, int pixelSize, Point2D p, RGBColor c);

    /** Draws text centered on a pixel position using its local visual bounds.
     * @param text Text to display.
     * @param pixelSize Character size in pixels.
     * @param p Center position in window coordinates.
     * @param c RGB fill color.
     * @return No value.
     */
    void DrawCenteredText(const std::string& text, int pixelSize, Point2D p, RGBColor c);

    /** Draws a filled circle centered at a pixel position.
     * @param p Circle center in window coordinates.
     * @param radius Circle radius in pixels.
     * @param c RGB fill color.
     * @return No value.
     */
    void DrawCircle(Point2D p, float radius, RGBColor c);

    /** Draws a filled axis-aligned rectangle.
     * @param r Rectangle bounds in window coordinates.
     * @param c RGB fill color.
     * @return No value.
     */
    void DrawRect(Rect r, RGBColor c);

    /** Draws only the outline of an axis-aligned rectangle.
     * @param r Rectangle bounds in window coordinates.
     * @param width Outline thickness in pixels.
     * @param c RGB outline color.
     * @return No value.
     */
    void FrameRect(Rect r, float width, RGBColor c);

    /** Draws a line segment as a filled quadrilateral; zero-length segments are ignored.
     * @param from Starting point in window coordinates.
     * @param to Ending point in window coordinates.
     * @param width Line thickness in pixels.
     * @param c RGB fill color.
     * @return No value.
     */
    void DrawLine(Point2D from, Point2D to, float width, RGBColor c);

    /** Gets the current render-window width in pixels.
     * @return Window width.
     */
    int GetWindowWidth();

    /** Gets the current render-window height in pixels.
     * @return Window height.
     */
    int GetWindowHeight();

private:
    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
    
    friend class GameEngine;
    void SetContextOffset(Point2D p);
    void SetContextRotation(float rotation);
    Point2D Transform(Point2D p);
    Point2D ReverseTransform(Point2D p);
    Rect Transform(Rect r);

    Point2D mContextOffset;
    float mContextRotation;

    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
};

}  // namespace CMPUT350

#endif  // RENDERTARGET_H
