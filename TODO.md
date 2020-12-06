- continue work on the range adjustments widget
- Make GraphDraw and mainGraph use the new settings:
    - update graphview to handle new approach and definitions
    - display tick coordinates properly following the settings (decimal global multiplier or not ...etc)
- Implement properly reset to default view in settings window for both the linear scale and log and for both axes,
basing off the base given by the user
- Make main graph use basegraphdraw (previously imagepreview)

Log view branch work:
- qpainter will do a graph scale and translation to match pixel units with graph units
- fill from range for seqs and functions

Export branch work:
- Make the actual exporting with the user given formats, the format left are image formats
- Implement tick spacing auto-update in export preview: easier in the new approach (simply update the base power)
- Unify the code for exportPreview and mainGraph into GraphDraw maybe.
- Change when the tick values disappear in the main graph on user move: draw bigger than the maingraph widget

- Fix bug with zooming in the main window, it doesn't work sometimes
- Enable adding additional pins in data tables to represent more than one curve per data table
- check if sequence draw with diffrent nmin work properly

Other work:
- update pictures in appdata/screenshots with new window layouts

Possible improvements:
- Change the curve saving from units in pixels to real units, scale the painter instead.
- Make a linestyle chooser widget based off a priori on QComboBox and images of line styles.
- Use it to offer the ability to change line styles for the grid and subgrid.
- Update the curve sampling method to an adaptive one: once the distance between points is bigger than the target in pixels, update.
