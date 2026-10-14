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
#include <QPainterPath>
#include <QPdfWriter>
#include <QStyleHints>
#include <QQuickWindow>
#include <QSvgGenerator>
#include <QSaveFile>
#include <QFileInfo>
#include <QImageWriter>
#include <QSGClipNode>
#include <QSGRectangleNode>
#include <QSGTextNode>
#include <QSGTransformNode>
#include <QTextLayout>

#include <map>
#include <ranges>
#include <tuple>

Graph::Graph(QQuickItem *parent)
  : QQuickItem(parent),
    MathObjectDraw(),
    settings(information->graphSettings),
    layoutEngine(settings, viewMapper)
{
  setFlag(ItemHasContents);
  pen.setCapStyle(Qt::RoundCap);

  viewMapper.setGraphRange(settings.range.getLatestValidSnapshot());

  connect(&settings, &ZeGraphSettings::minPointsLg2Changed, this, &Graph::minMaxPointsChanged);
  connect(&settings, &ZeGraphSettings::maxPointsLg2Changed, this, &Graph::minMaxPointsChanged);
  // main() loads the documents before QML creates the graph
  minMaxPointsChanged();

  connect(&settings, &ZeGraphSettings::sizeSettingsChanged, this, [this]{ polish(); });
  connect(&settings, &ZeGraphSettings::zoomSettingsChanged, this, [this]{ polish(); });
  connect(&settings, &ZeGraphSettings::axesSettingsChanged, this, [this]{ polish(); });
  connect(&settings, &ZeGraphSettings::gridSettingsChanged, this, [this]{ polish(); });
  connect(&settings, &ZeGraphSettings::subgridSettingsChanged, this, [this]{ polish(); });
  connect(&settings, &ZeGraphSettings::backgroundColorChanged, this, [this]{ polish(); });
  connect(&settings, &ZeGraphSettings::fontChanged, this, [this]{ polish(); });
  connect(&settings, &ZeGraphSettings::pixelDensityChanged, this, [this]{ polish(); });
  connect(&zg::mathWorld, &zg::MathWorld::updated, this, [this]{ polish(); });
  connect(qGuiApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this]{ polish(); });
  connect(&zg::animationConductor, &zg::AnimationConductor::tick, this, [this]{ polish(); });

  emit settingsChanged();
}

void Graph::setPainterFont(QFont font)
{
  // QPainter converts points to pixels with its device's logicalDpiY, and the
  // layout sizes its fonts for 72 dpi
  font.setPointSizeF(font.pointSizeF() * 72. / painter->device()->logicalDpiY());
  painter->setFont(font);
}

void Graph::paint(QPainter* p, Export target)
{
  painter = p;

  const bool vector = target != Export::Image;
  const double dpr = vector ? 1. : window()->effectiveDevicePixelRatio();

  const double scale = vector
    ? settings.getSize().scalingFactor * p->device()->logicalDpiX()
        / (ZeGraphSettings::targetPixelDensity * CM_PER_INCH)
    : settings.getTotalScaleFactor();

  painter->scale(scale * dpr, scale * dpr);
  scaledSize = p->window().size().toSizeF() / dpr / scale;
  supportRect = QRectF(QPointF(), scaledSize);
  drawSupport();

  layout = layoutEngine.compute(supportRect);

  painter->translate(layout.graph.topLeft());
  painter->setClipRect(QRectF(QPointF(), layout.graph.size()));

  for (const LineBatch& batch : layout.belowCurves)
    drawLines(batch);

  sampler.update();

  drawObjects();

  for (const LineBatch& batch : layout.aboveCurves)
    drawLines(batch);

  painter->setClipping(false);

  drawLabels(target == Export::Svg);
  drawLines(layout.border);

  painter = nullptr;

  // the export moved the view mapper and samples to its own sheet
  polish();
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

void Graph::drawLabels(bool asPaths)
{
  painter->setPen(layout.textColor);

  for (const Label& label : layout.labels)
  {
    setPainterFont(layout.font(label.role));
    painter->save();
    painter->translate(label.origin);
    painter->rotate(label.angle);
    if (asPaths)
    {
      QPainterPath path;
      path.addText(QPointF(), painter->font(), label.text);
      painter->fillPath(path, layout.textColor);
    }
    else painter->drawText(QPointF(), label.text);
    painter->restore();
  }
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

  paint(&imagePainter, Export::Image);

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

  pen.setStyle(Qt::NoPen);
  painter->setPen(pen);

  painter->drawRect(supportRect);
}

void Graph::computeSupportRect()
{
  QRectF rect;

  if (settings.getSize().sheetFillsWindow or settings.getZoom().zoomingType == ZoomingType::CUSTOM)
  {
    rect.setSize(scaledSize);
  }
  else if (settings.getZoom().zoomingType == ZoomingType::FITSHEET)
  {
    const double ratio = scaledSize.height() / scaledSize.width();
    const double targetRatio = settings.getSize().cmSheetSize.height() / settings.getSize().cmSheetSize.width();

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

  paint(&pdfPainter, Export::Pdf);
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

  paint(&svgPainter, Export::Svg);

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

  polish();
}

void Graph::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
  QQuickItem::geometryChange(newGeometry, oldGeometry);
  if (newGeometry.size() != oldGeometry.size())
    polish();
}

void Graph::itemChange(ItemChange change, const ItemChangeData& value)
{
  QQuickItem::itemChange(change, value);
  if (change == ItemDevicePixelRatioHasChanged or change == ItemSceneChange)
    polish();
}

void Graph::updatePolish()
{
  if (width() <= 0 or height() <= 0 or not window())
    return;

  totalScaleFactor = settings.getTotalScaleFactor();
  scaledSize = size() / totalScaleFactor;
  computeSupportRect();

  Frame::Source source{
    .scale = float(totalScaleFactor),
    .pxPerUnit = float(totalScaleFactor * window()->effectiveDevicePixelRatio()),
    .background = settings.getBackgroundColor().getCurrent(),
    .layout = layoutEngine.compute(supportRect),
  };
  const float pxPerUnit = source.pxPerUnit;

  sampler.update();

  // an animation only moves the curves
  if (source != frame.source)
  {
    auto lines = [&](const LineBatch& batch) {
      LineGeometry g(batch.color, batch.width, {}, pxPerUnit);
      g.addSegments(batch.lines);
      return g;
    };

    frame.belowCurves = source.layout.belowCurves | std::views::transform(lines) | std::ranges::to<std::vector>();
    frame.aboveCurves = source.layout.aboveCurves | std::views::transform(lines) | std::ranges::to<std::vector>();
    frame.border = lines(source.layout.border);
    frame.source = std::move(source);
    frame.layoutDirty = true;
  }

  frame.curves.clear();
  frame.markers.clear();
  for (const zg::SampledCurve& curve : sampler.getCurves())
  {
    const zg::CurveStyle& style = curve.style;
    if (not style.visible or curve.curve.empty())
      continue;

    if (style.drawLine)
    {
      LineGeometry g(style.color, style.lineWidth, style.dashPattern, pxPerUnit);
      for (const QPolygonF& polyline : buildFinalCurve(curve))
        g.addPolyline(polyline);
      frame.curves.push_back(std::move(g));
    }

    if (curve.discrete and style.pointStyle != zg::PlotStyle::None)
      frame.markers.push_back(MarkerGeometry{curve.px_curve, style, pxPerUnit});
  }
  frame.curvesDirty = true;

  update();
}

namespace {

/// @brief a label's text node, kept while its text, font and color don't change
class LabelNode : public QSGTransformNode
{
public:
  using Key = std::tuple<QString, QFont, QRgb>;

  explicit LabelNode(Key key) : key(std::move(key)) {}

  const Key key;
};

/// @brief the graph's scene graph, kept from frame to frame
///
/// root (scale) -> background
///              -> origin (top left corner of the graph) -> clip -> lines and markers
///                                                       -> border
///                                                       -> labels (unscaled)
class GraphNode : public QSGTransformNode
{
public:
  explicit GraphNode(QQuickWindow* window)
    : window(window)
  {
    background = window->createRectangleNode();
    appendChildNode(background);

    appendChildNode(origin);

    clip->setIsRectangular(true);
    clip->setGeometry(new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 4));
    clip->setFlag(OwnsGeometry);
    origin->appendChildNode(clip);
    for (QSGNode* layer : {below, curves, markers, above})
      clip->appendChildNode(layer);

    origin->appendChildNode(border);
    origin->appendChildNode(labels);
  }

  void sync(const Graph::Frame& f)
  {
    if (f.curvesDirty)
    {
      syncChildren<LineNode>(curves, f.curves);
      syncChildren<MarkerNode>(markers, f.markers);
    }

    if (not f.layoutDirty)
      return;

    const Graph::Frame::Source& source = f.source;
    const GraphLayout& layout = source.layout;

    QMatrix4x4 scale;
    scale.scale(source.scale);
    setMatrix(scale);

    background->setRect(layout.figure);
    background->setColor(source.background);

    QMatrix4x4 translation;
    translation.translate(float(layout.graph.x()), float(layout.graph.y()));
    origin->setMatrix(translation);

    const QRectF clipRect(QPointF(), layout.graph.size());
    QSGGeometry::updateRectGeometry(clip->geometry(), clipRect);
    clip->setClipRect(clipRect);
    clip->markDirty(DirtyGeometry);

    syncChildren<LineNode>(below, f.belowCurves);
    syncChildren<LineNode>(above, f.aboveCurves);
    border->set(f.border);

    syncLabels(layout, source.scale);
  }

protected:
  /// @brief one child node per item in 'data', reusing existing children
  template <class Node, class Data>
  static void syncChildren(QSGNode* parent, const std::vector<Data>& data)
  {
    QSGNode* child = parent->firstChild();
    for (const Data& d : data)
    {
      if (not child)
      {
        child = new Node;
        parent->appendChildNode(child);
      }
      static_cast<Node*>(child)->set(d);
      child = child->nextSibling();
    }

    while (child)
    {
      QSGNode* next = child->nextSibling();
      parent->removeChildNode(child);
      delete child;
      child = next;
    }
  }

  /// @brief a label keeps its text node while its text doesn't change, and
  ///        only moves
  ///
  /// Native text rasterizes each glyph at its font size, so a label undoes the
  /// root's scale and scales its font instead.
  void syncLabels(const GraphLayout& layout, float scale)
  {
    std::multimap<LabelNode::Key, LabelNode*> unused;
    for (QSGNode* child = labels->firstChild(); child; child = child->nextSibling())
    {
      auto* node = static_cast<LabelNode*>(child);
      unused.emplace(node->key, node);
    }

    for (const Label& label : layout.labels)
    {
      QFont font = layout.font(label.role);
      font.setPointSizeF(font.pointSizeF() * scale);
      // native glyphs are cached unrotated, so a turned label would get its
      // subpixel color fringes along the wrong axis
      if (label.angle != 0)
        font.setStyleStrategy(QFont::NoSubpixelAntialias);
      LabelNode::Key key{label.text, font, layout.textColor.rgba()};
      LabelNode* node;
      if (auto it = unused.find(key); it != unused.end())
      {
        node = it->second;
        unused.erase(it);
      }
      else
      {
        node = new LabelNode(std::move(key));
        node->appendChildNode(textNode(label, font, layout.textColor));
        labels->appendChildNode(node);
      }

      QMatrix4x4 m;
      m.translate(float(label.origin.x()), float(label.origin.y()));
      m.rotate(float(label.angle), 0, 0, 1);
      m.scale(1.f / scale);
      node->setMatrix(m);
    }

    for (LabelNode* node : unused | std::views::values)
    {
      labels->removeChildNode(node);
      delete node;
    }
  }

  /// @brief the text of 'label', its baseline starting at (0, 0)
  QSGTextNode* textNode(const Label& label, const QFont& font, QColor color) const
  {
    QTextLayout text(label.text, font, GraphLayoutEngine::referenceDevice());
    text.beginLayout();
    QTextLine line = text.createLine();
    line.setNumColumns(int(label.text.size()));
    text.endLayout();

    QSGTextNode* node = window->createTextNode();
    node->setRenderType(QSGTextNode::NativeRendering);
    node->setColor(color);
    node->addTextLayout(QPointF(0, -line.ascent()), &text);
    return node;
  }

  QQuickWindow* window;
  QSGRectangleNode* background;
  QSGTransformNode* origin = new QSGTransformNode;
  QSGClipNode* clip = new QSGClipNode;
  QSGNode* below = new QSGNode;
  QSGNode* curves = new QSGNode;
  QSGNode* markers = new QSGNode;
  QSGNode* above = new QSGNode;
  LineNode* border = new LineNode;
  QSGNode* labels = new QSGNode;
};

} // namespace

QSGNode* Graph::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
  auto* root = static_cast<GraphNode*>(oldNode);
  if (not root)
  {
    root = new GraphNode(window());
    frame.layoutDirty = frame.curvesDirty = true;
  }

  root->sync(frame);
  frame.layoutDirty = frame.curvesDirty = false;
  return root;
}
