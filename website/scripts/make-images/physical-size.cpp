// make-images.py preloads this library into the app it captures. KWin doesn't
// give the --virtual screen a physical size, so Qt reports 100 dots per inch.
// The app draws its graph in centimeters (ZeGraphSettings::screenChanged), so
// graph lines and text would come out at another size than on the screen the
// pictures are measured on. QScreen::physicalSize() returns that screen's size
// instead: WIDTH_MM x HEIGHT_MM, which make-images.py passes.

#include <QScreen>

QSizeF QScreen::physicalSize() const
{
  return QSizeF(WIDTH_MM, HEIGHT_MM);
}
