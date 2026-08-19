// make-images.py preloads this library into the app it captures. KWin gives the
// screen of --virtual no physical size, and Qt then reports 100 dots per inch.
// The app draws its graph in centimeters (ZeGraphSettings::screenChanged), so
// the lines and the text of a graph would come out another size than on the
// screen the pictures are measured on. QScreen::physicalSize() returns the size
// of that screen instead: WIDTH_MM x HEIGHT_MM, which make-images.py passes.

#include <QScreen>

QSizeF QScreen::physicalSize() const
{
  return QSizeF(WIDTH_MM, HEIGHT_MM);
}
