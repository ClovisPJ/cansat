(* Content-type: application/vnd.wolfram.cdf.text *)

(*** Wolfram CDF File ***)
(* http://www.wolfram.com/cdf *)

(* CreatedBy='Mathematica 10.0' *)

(*************************************************************************)
(*                                                                       *)
(*  The Mathematica License under which this file was created prohibits  *)
(*  restricting third parties in receipt of this file from republishing  *)
(*  or redistributing it by any means, including but not limited to      *)
(*  rights management or terms of use, without the express consent of    *)
(*  Wolfram Research, Inc. For additional information concerning CDF     *)
(*  licensing and redistribution see:                                    *)
(*                                                                       *)
(*        www.wolfram.com/cdf/adopting-cdf/licensing-options.html        *)
(*                                                                       *)
(*************************************************************************)

(*CacheID: 234*)
(* Internal cache information:
NotebookFileLineBreakTest
NotebookFileLineBreakTest
NotebookDataPosition[      1064,         20]
NotebookDataLength[      7843,        172]
NotebookOptionsPosition[      8335,        165]
NotebookOutlinePosition[      8848,        187]
CellTagsIndexPosition[      8805,        184]
WindowFrame->Normal*)

(* Beginning of Notebook Content *)
Notebook[{
Cell[BoxData[
 TagBox[
  StyleBox[
   DynamicModuleBox[{$CellContext`a$$ = 0.1, $CellContext`c$$ = 
    0.5, $CellContext`e$$ = 0.01, $CellContext`g$$ = 9.8, $CellContext`m$$ = 
    0.5, $CellContext`max$$ = 10, $CellContext`p$$ = 50, $CellContext`T$$ = 
    0.00001, $CellContext`\[Rho]$$ = 1.225, Typeset`show$$ = True, 
    Typeset`bookmarkList$$ = {}, Typeset`bookmarkMode$$ = "Menu", 
    Typeset`animator$$, Typeset`animvar$$ = 1, Typeset`name$$ = 
    "\"untitled\"", Typeset`specs$$ = {{{
       Hold[$CellContext`T$$], 0.00001, "Time"}, 0.00001, 10000}, {{
       Hold[$CellContext`max$$], 10, "Height at Start"}, 2, 100}, {{
       Hold[$CellContext`p$$], 50, "Motor Power"}, 0, 100}, {{
       Hold[$CellContext`c$$], 0.5, "Drag Coefficent"}, 0, 1}, {{
       Hold[$CellContext`a$$], 0.1, "Cross Sectional Area"}, 0.01, 0.3}, {{
       Hold[$CellContext`m$$], 0.5, "Mass"}, 0.2, 1}, {{
       Hold[$CellContext`e$$], 0.01, "Motor Efficiency"}, 0.001, 0.2}, {{
       Hold[$CellContext`\[Rho]$$], 1.225, "Density of Air"}, 0.1, 5}, {{
       Hold[$CellContext`g$$], 9.8, "Acceleration of Gravity"}, 0.1, 30}}, 
    Typeset`size$$ = {381., {90., 97.}}, Typeset`update$$ = 0, 
    Typeset`initDone$$, Typeset`skipInitDone$$ = True, $CellContext`T$1368$$ =
     0, $CellContext`max$1369$$ = 0, $CellContext`p$1370$$ = 
    0, $CellContext`c$1371$$ = 0, $CellContext`a$1372$$ = 
    0, $CellContext`m$1373$$ = 0, $CellContext`e$1374$$ = 
    0, $CellContext`\[Rho]$1375$$ = 0, $CellContext`g$1376$$ = 0}, 
    DynamicBox[Manipulate`ManipulateBoxes[
     1, StandardForm, 
      "Variables" :> {$CellContext`a$$ = 0.1, $CellContext`c$$ = 
        0.5, $CellContext`e$$ = 0.01, $CellContext`g$$ = 
        9.8, $CellContext`m$$ = 0.5, $CellContext`max$$ = 
        10, $CellContext`p$$ = 50, $CellContext`T$$ = 
        0.00001, $CellContext`\[Rho]$$ = 1.225}, "ControllerVariables" :> {
        Hold[$CellContext`T$$, $CellContext`T$1368$$, 0], 
        Hold[$CellContext`max$$, $CellContext`max$1369$$, 0], 
        Hold[$CellContext`p$$, $CellContext`p$1370$$, 0], 
        Hold[$CellContext`c$$, $CellContext`c$1371$$, 0], 
        Hold[$CellContext`a$$, $CellContext`a$1372$$, 0], 
        Hold[$CellContext`m$$, $CellContext`m$1373$$, 0], 
        Hold[$CellContext`e$$, $CellContext`e$1374$$, 0], 
        Hold[$CellContext`\[Rho]$$, $CellContext`\[Rho]$1375$$, 0], 
        Hold[$CellContext`g$$, $CellContext`g$1376$$, 0]}, 
      "OtherVariables" :> {
       Typeset`show$$, Typeset`bookmarkList$$, Typeset`bookmarkMode$$, 
        Typeset`animator$$, Typeset`animvar$$, Typeset`name$$, 
        Typeset`specs$$, Typeset`size$$, Typeset`update$$, Typeset`initDone$$,
         Typeset`skipInitDone$$}, "Body" :> Row[{
         Graphics3D[{
           ReplaceAll[
            
            Cylinder[{{$CellContext`x 0.001, 
               0, (-$CellContext`z) 
                0.001 + $CellContext`max$$}, {$CellContext`x 0.001 + 1, 
               0, (-$CellContext`z) 0.001 + $CellContext`max$$}}, 0.2], {
             Part[
              Part[
               Solve[
                
                And[$CellContext`m$$ $CellContext`T$$ $CellContext`x^2 + 
                  4 $CellContext`c$$ $CellContext`a$$ $CellContext`\[Rho]$$ \
$CellContext`T$$ $CellContext`x^3 == $CellContext`p$$ $CellContext`e$$ \
$CellContext`T$$^4, $CellContext`x >= 0], Reals], 1], 1], 
             Part[
              Part[
               Solve[
                
                And[$CellContext`m$$ 9.8 $CellContext`T$$^3 - (3/
                   2) $CellContext`\[Rho]$$ $CellContext`c$$ $CellContext`a$$ \
$CellContext`T$$ $CellContext`z^2 == 
                 6 $CellContext`m$$ $CellContext`z $CellContext`T$$, \
$CellContext`z >= 0], Reals], 1], 1]}]}, 
          PlotRange -> {{0, 10}, {-1, 1}, {-1, $CellContext`max$$ + 1}}, Axes -> 
          False], 
         Column[{
           Row[{
             Text[
             "Max Speed: "], (
              2 $CellContext`e$$ $CellContext`p$$/($CellContext`\[Rho]$$ \
$CellContext`c$$ $CellContext`a$$))^(1/3)}], 
           Text["v/t and s/t graphs for motion in x plane"], 
           GraphicsRow[{
             
             ContourPlot[$CellContext`p$$ $CellContext`e$$ $CellContext`t - (
                1/2) $CellContext`\[Rho]$$ $CellContext`c$$ $CellContext`a$$ \
$CellContext`v^3 $CellContext`t == (1/
                2) $CellContext`v^2 $CellContext`m$$, {$CellContext`t, 0, 
               100}, {$CellContext`v, 0, 5}, FrameLabel -> Automatic], 
             
             ContourPlot[$CellContext`m$$ $CellContext`t $CellContext`s^2 + 
               4 $CellContext`c$$ $CellContext`a$$ $CellContext`\[Rho]$$ \
$CellContext`t $CellContext`s^3 == $CellContext`p$$ $CellContext`e$$ \
$CellContext`t^4, {$CellContext`t, 0, 100}, {$CellContext`s, 0, 200}, 
              FrameLabel -> Automatic]}], 
           Row[{
             Text[
             "Terminal Velocity: "], (
              2 $CellContext`m$$ $CellContext`g$$/($CellContext`\[Rho]$$ \
$CellContext`c$$ $CellContext`a$$))^(1/3)}], 
           Text["v/t and s/t graphs for motion in z plane"], 
           GraphicsRow[{
             
             ContourPlot[$CellContext`m$$ $CellContext`g$$ $CellContext`t - (
                1/2) $CellContext`\[Rho]$$ $CellContext`v^2 $CellContext`c$$ \
$CellContext`a$$ $CellContext`t == $CellContext`m$$ $CellContext`v, \
{$CellContext`t, 0, 100}, {$CellContext`v, 0, 20}, FrameLabel -> Automatic], 
             
             ContourPlot[$CellContext`m$$ $CellContext`g$$ $CellContext`t^3 - \
(3/2) $CellContext`\[Rho]$$ $CellContext`c$$ $CellContext`a$$ $CellContext`t \
$CellContext`s^2 == 
              6 $CellContext`m$$ $CellContext`s $CellContext`t, \
{$CellContext`t, 0, 100}, {$CellContext`s, 0, 200}, FrameLabel -> 
              Automatic]}]}], Null}], 
      "Specifications" :> {{{$CellContext`T$$, 0.00001, "Time"}, 0.00001, 
         10000}, {{$CellContext`max$$, 10, "Height at Start"}, 2, 
         100}, {{$CellContext`p$$, 50, "Motor Power"}, 0, 
         100}, {{$CellContext`c$$, 0.5, "Drag Coefficent"}, 0, 
         1}, {{$CellContext`a$$, 0.1, "Cross Sectional Area"}, 0.01, 
         0.3}, {{$CellContext`m$$, 0.5, "Mass"}, 0.2, 
         1}, {{$CellContext`e$$, 0.01, "Motor Efficiency"}, 0.001, 
         0.2}, {{$CellContext`\[Rho]$$, 1.225, "Density of Air"}, 0.1, 
         5}, {{$CellContext`g$$, 9.8, "Acceleration of Gravity"}, 0.1, 30}}, 
      "Options" :> {}, "DefaultOptions" :> {}],
     ImageSizeCache->{787., {123., 130.}},
     SingleEvaluation->True],
    Deinitialization:>None,
    DynamicModuleValues:>{},
    SynchronousInitialization->True,
    UnsavedVariables:>{Typeset`initDone$$},
    UntrackedVariables:>{Typeset`size$$}], "Manipulate",
   Deployed->True,
   StripOnInput->False],
  Manipulate`InterpretManipulate[1]]], "Output", "PluginEmbeddedContent"]
},
WindowSize->{838.46, 262.65000000000003`},
Visible->True,
AuthoredSize->{838, 263},
ScrollingOptions->{"HorizontalScrollRange"->Fit,
"VerticalScrollRange"->Fit},
ShowCellBracket->False,
Deployed->True,
CellContext->Notebook,
TrackCellChangeTimes->False,
FrontEndVersion->"10.0 for Linux x86 (64-bit) (June 27, 2014)",
StyleDefinitions->"Default.nb"
]
(* End of Notebook Content *)

(* Internal cache information *)
(*CellTagsOutline
CellTagsIndex->{}
*)
(*CellTagsIndex
CellTagsIndex->{}
*)
(*NotebookFileOutline
Notebook[{
Cell[1464, 33, 6867, 130, 255, "Output"]
}
]
*)

(* End of internal cache information *)

(* NotebookSignature 9uTw2OY#rwdlKB1b8FXw#NM2 *)
