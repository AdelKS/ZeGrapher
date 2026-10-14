/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "GraphDraw/graph.h"
#include "Animation/animationconductor.h"
#include "information.h"
#include "buildconfig.h"

#include <QPageLayout>
#include <QPdfWriter>
#include <QStyleHints>
#include <QQuickWindow>
#include <QSvgGenerator>
#include <QSaveFile>
#include <QFileInfo>
#include <QImageWriter>

Graph::Graph(QQuickItem *parent)
  : QQuickPaintedItem(parent),
    MathObjectDraw(),
    settings(information->graphSettings),
    layoutEngine(settings, viewMapper)
{
  pen.setCapStyle(Qt::RoundCap);

  viewMapper.setGraphRange(settings.range.getLatestValidSnapshot());

  connect(&settings, &ZeGraphSettings::minPointsLg2Changed, this, &Graph::minMaxPointsChanged);
  connect(&settings, &ZeGraphSettings::maxPointsLg2Changed, this, &Graph::minMaxPointsChanged);
  // main() loads the documents before QML creates the graph
  minMaxPointsChanged();

  connect(&settings, &ZeGraphSettings::sizeSettingsChanged, this, [this]{ update(); });
  connect(&settings, &ZeGraphSettings::zoomSettingsChanged, this, [this]{ update(); });
  connect(&settings, &ZeGraphSettings::axesSettingsChanged, this, [this]{ update(); });
  connect(&settings, &ZeGraphSettings::gridSettingsChanged, this, [this]{ update(); });
  connect(&settings, &ZeGraphSettings::subgridSettingsChanged, this, [this]{ update(); });
  connect(&settings, &ZeGraphSettings::backgroundColorChanged, this, [this]{ update(); });
  connect(&settings, &ZeGraphSettings::fontChanged, this, [this]{ update(); });
  connect(&settings, &ZeGraphSettings::pixelDensityChanged, this, [this]{ update(); });
  connect(&zg::mathWorld, &zg::MathWorld::updated, this, [this]{ update(); });
  connect(qGuiApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this]{ update(); });
  connect(&zg::animationConductor, &zg::AnimationConductor::tick, this, [this]{ update(); });

  emit settingsChanged();
}

void Graph::setPainterFont(QFont font)
{
  // QPainter converts points to pixels with its device's logicalDpiY, and the
  // layout sizes its fonts for 72 dpi
  font.setPointSizeF(font.pointSizeF() * 72. / painter->device()->logicalDpiY());
  painter->setFont(font);
}

void Graph::paint(QPainter *p)
{
  painter = p;

  if (paintType == VECTOR_EXPORT)
    scaledSize = p->window().size().toSizeF();
  else scaledSize = p->window().size().toSizeF() / window()->effectiveDevicePixelRatio();

  scaleView();
  drawSupport();

  drawAll();

  painter = nullptr;
}

void Graph::drawAll()
{
  layout = layoutEngine.compute(supportRect);

  painter->translate(layout.graph.topLeft());
  painter->setClipRect(QRectF(QPointF(), layout.graph.size()));

  for (const LineBatch& batch : layout.belowCurves)
    drawLines(batch);

  sampler.update();

  if (paintType == GPU_RENDER)
    updateQmlData();
  else drawObjects();

  for (const LineBatch& batch : layout.aboveCurves)
    drawLines(batch);

  painter->setClipping(false);

  drawLabels();
  drawLines(layout.border);
}

void Graph::drawLines(const LineBatch& batch)
{
  pen.setColor(batch.color);
  pen.setWidthF(batch.width);
  pen.setStyle(Qt::SolidLine);
  pen.setCapStyle(batch.cap);
  pen.setJoinStyle(Qt::RoundJoin);
  painter->setPen(pen);
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->drawLines(batch.lines.data(), int(batch.lines.size()));
}

void Graph::drawLabels()
{
  painter->setPen(layout.textColor);

  for (const Label& label : layout.labels)
  {
    setPainterFont(layout.font(label.role));
    painter->save();
    painter->translate(label.origin);
    painter->rotate(label.angle);
    painter->drawText(QPointF(), label.text);
    painter->restore();
  }
}

void Graph::scaleView()
{
  totalScaleFactor = paintType == VECTOR_EXPORT
    ? settings.getSize().scalingFactor * painter->device()->logicalDpiX()
        / (ZeGraphSettings::targetPixelDensity * CM_PER_INCH)
    : settings.getTotalScaleFactor();

  // the qpainter is still drawing to the actual buffer-size that's given by painter-viewport
  // this is just telling it to draw everything bigger and translate coordinates accordingly
  painter->scale(totalScaleFactor, totalScaleFactor);

  if (paintType == IMAGE_EXPORT)
    painter->scale(window()->devicePixelRatio(), window()->devicePixelRatio());

  // this is the new "view size": i.e. the whole widget spans that size, drawing anything above goes out
  scaledSize /= totalScaleFactor;
}

void Graph::exportImage(QUrl filename)
{
  const QString path = filename.toLocalFile();
  const QSize pixels = (size() * window()->devicePixelRatio()).toSize();
  QImage image(pixels, QImage::Format_RGB32);

  if (image.isNull())
  {
    information->exportFailed(path, tr("An image of %1×%2 pixels does not fit in memory.")
                                      .arg(pixels.width()).arg(pixels.height()));
    return;
  }

  image.fill(settings.getBackgroundColor().getCurrent().rgb());

  QPainter imagePainter(&image);

  paintType = IMAGE_EXPORT;
  paint(&imagePainter);
  paintType = GPU_RENDER;

  // QImageWriter picks the format from the file suffix. error() tells an
  // unknown suffix apart from a failed write, such as to a read-only folder
  QImageWriter writer(path);
  writer.setQuality(100);

  if (not writer.write(image))
  {
    const QString suffix = QFileInfo(path).suffix();

    if (writer.error() != QImageWriter::UnsupportedFormatError)
      information->exportFailed(path, writer.errorString());
    else if (suffix.isEmpty())
      information->exportFailed(path, tr("The file name has no extension, so no "
                                         "image format matches it."));
    else
      information->exportFailed(path, tr("No image format matches the extension '%1'.")
                                        .arg(suffix));
  }
}

void Graph::drawSupport()
{ // draws the sheet on an untransformed view
  painter->setBrush(QBrush(settings.getBackgroundColor().getCurrent()));

  computeSupportRect();

  pen.setStyle(Qt::NoPen);
  painter->setPen(pen);

  painter->drawRect(supportRect);
}

void Graph::computeSupportRect()
{
  QRectF rect;

  if (paintType != GPU_RENDER or settings.getSize().sheetFillsWindow or settings.getZoom().zoomingType == ZoomingType::CUSTOM)
  {
    rect.setSize(scaledSize);
  }
  else if (settings.getZoom().zoomingType == ZoomingType::FITSHEET)
  {
    double ratio, targetRatio;
    ratio = scaledSize.height() / scaledSize.width();

    if (settings.getSize().sizeUnit == SizeUnit::CENTIMETER)
      targetRatio = settings.getSize().cmSheetSize.height() / settings.getSize().cmSheetSize.width();
    else
      targetRatio = double(settings.getSize().pxSheetSize.height())
                    / double(settings.getSize().pxSheetSize.width());

    if (ratio <= targetRatio)
    {
      rect.setHeight(scaledSize.height());
      rect.setWidth(scaledSize.height() / targetRatio);

      rect.moveTopLeft(QPointF((scaledSize.width() - rect.width()) / 2, 0));
    }
    else
    {
      rect.setHeight(scaledSize.width() * targetRatio);
      rect.setWidth(scaledSize.width());

      rect.moveTopLeft(QPoint(0, (scaledSize.height() - rect.height()) / 2));
    }
  }

  supportRect = rect;
}

void Graph::updateQmlData()
{
  qmlData.clear();

  for (const zg::SampledCurve& f_curve : sampler.getCurves())
  {
    if (not f_curve.style.visible) continue;
    if (f_curve.curve.empty()) continue;

    QVariantMap curve;
    if (f_curve.style.drawLine)
    {
      const QList<QPolygonF> segments = buildFinalCurve(f_curve, totalScaleFactor);

      curve[QStringLiteral("segments")] = QVariant::fromValue(std::move(segments));
      curve[QStringLiteral("style")]    = QVariant::fromValue(f_curve.style);

    }

    if (f_curve.discrete and f_curve.style.pointStyle != zg::PlotStyle::None)
      curve[QStringLiteral("markers")] = QVariant::fromValue(&f_curve);
    else curve[QStringLiteral("markers")] = QVariant::fromValue(static_cast<const zg::SampledCurve*>(nullptr));

    if (not curve.isEmpty())
      qmlData.append(std::move(curve));
  }

  // Graph rect position in item pixel coordinates
  graphRect = QRectF(layout.graph.topLeft() * totalScaleFactor,
                     layout.graph.size() * totalScaleFactor);

  emit qmlDataChanged();
  emit graphRectChanged();
}

void Graph::exportPDF(QUrl fileName)
{
  QPdfWriter pdfWriter(fileName.toLocalFile());

  pdfWriter.setCreator(QString("ZeGrapher ") + SOFTWARE_VERSION);
  pdfWriter.setTitle(tr("Exported graph"));

  pdfWriter.setResolution(1200);

  QPageLayout layout(
    QPageSize(settings.getSize().cmSheetSize * 10., QPageSize::Millimeter, "", QPageSize::ExactMatch),
    QPageLayout::Portrait,
    QMarginsF(0, 0, 0, 0),
    QPageLayout::Millimeter
  );

  if (not layout.isValid()) {
    const QSizeF sheet = settings.getSize().cmSheetSize;
    information->exportFailed(fileName.toLocalFile(),
                              tr("A page of %1×%2 cm is not a valid page size.")
                                .arg(sheet.width()).arg(sheet.height()));
    return;
  }

  pdfWriter.setPageLayout(layout);
  QPainter pdfPainter(&pdfWriter);

  // QPdfWriter opens the file, and the painter stays inactive when it could not
  if (not pdfPainter.isActive())
  {
    information->exportFailed(fileName.toLocalFile());
    return;
  }

  paintType = VECTOR_EXPORT;
  paint(&pdfPainter);
  paintType = GPU_RENDER;
}

void Graph::exportSVG(QUrl fileName)
{
  const QString path = fileName.toLocalFile();
  QSaveFile file(path);
  if (not file.open(QIODevice::WriteOnly))
  {
    information->exportFailed(path, file.errorString());
    return;
  }

  // SVG Tiny 1.2, the default, has no clipping
  QSvgGenerator svgGenerator(QSvgGenerator::SvgVersion::Svg11);
  svgGenerator.setOutputDevice(&file);

  svgGenerator.setTitle(tr("Exported graph"));
  //: %1 is the version of ZeGrapher, for example 4.0
  svgGenerator.setDescription(tr("Created with ZeGrapher %1").arg(SOFTWARE_VERSION));

  svgGenerator.setResolution(96);

  const QSize sheet = (settings.getSize().cmSheetSize / CM_PER_INCH * svgGenerator.resolution()).toSize();
  svgGenerator.setSize(sheet);
  svgGenerator.setViewBox(QRect(QPoint(0, 0), sheet));

  QPainter svgPainter(&svgGenerator);

  if (not svgPainter.isActive())
  {
    information->exportFailed(path);
    return;
  }

  paintType = VECTOR_EXPORT;
  paint(&svgPainter);
  paintType = GPU_RENDER;

  // QSvgGenerator writes the file when its painter ends.
  // QSaveFile::commit() also returns false when a write failed
  svgPainter.end();
  if (not file.commit())
    information->exportFailed(path, file.errorString());
}

void Graph::minMaxPointsChanged()
{
  sampler.min_points = 1ULL << settings.getMinPointsLg2();
  sampler.max_points = 1ULL << settings.getMaxPointsLg2();

  update();
}
