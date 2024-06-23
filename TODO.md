- Change graph curve quality setting so it defines the smallest pixel distance between two calculated points
- grid calculator: rework usage TARGET_TICKS_NUM (is it actually relevant ?)

- Make GraphDraw and mainGraph use the new settings:
    - update graphview to handle new approach and definitions
    - display tick coordinates properly following the settings (decimal global multiplier or not ...etc)
- Implement properly reset to default view in settings window for both the linear scale and log and for both axes,
basing off the base given by the user

Exporting
- Make the actual exporting with the user given formats, the format left are image formats
- Implement tick spacing auto-update in export preview: easier in the new approach (simply update the base power)
- Enable adding additional pins in data tables to represent more than one curve per data table

Other work:
- update pictures in appdata/screenshots with new window layouts

Possible improvements:
- Make a linestyle chooser widget based off a priori on QComboBox and images of line styles.
  - Use it to offer the ability to change line styles for the grid and subgrid.

Potentially useful QML classes
- Frame
- SwipeView
