/* A "proof of concept" project to replace the STM32F103 "Blue Pill"
   which is used in the "Magnetic Loop Controller" described in the book
   "Microcontroller Projects for Amateur Radio by Jack Purdum, W8TEE, and
   Albert Peter, AC8GY" with the Raspberry Pi Pico.
   Copyright (C) 2022  Gregory Raven

                                                    LICENSE AGREEMENT

  This program source code and its associated hardware design at subject to the GNU General Public License version 2,
                  https://opensource.org/licenses/GPL-2.0
  with the following additional conditions:
    1. Any commercial use of the hardware or software is prohibited without express, written, permission of the authors.
    2. This entire comment, unaltered, must appear at the top of the primary source file. In the Arduino IDE environemnt, this comment must
       appear at the top of the INO file that contains setup() and loop(). In any other environmentm, it must appear in the file containing
       main().
    3. This notice must appear in any derivative work, regardless of language used.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    A copy of the GPL-2.0 license is included in the repository as file LICENSE.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "GraphPlot.h"

GraphPlot::GraphPlot(Adafruit_ILI9341 &tft, DDS &dds, Data &data) : tft(tft), dds(dds), data(data)
{
}

/*****
  Purpose: To display the axes for a graph.
           The axes are drawn from the top down per display convention.
  Parameter list:
    int whichBandOption     // The band being used
  Return value:
    void
*****/
void GraphPlot::GraphAxis(int whichBandOption) // al modified 9-8-19
{
  tft.setTextSize(1);
  tft.setFont(&FreeSerif9pt7b);
  char buff[10];
  int chunks, tickCount;
  int yTick, xTick, yDotIncrement, xDotIncrement;
  int tcolor, bcolor;
  int i, k;
  float freqCount, freqEnd, pip;
  //  This needs update to use band limit variables, not hard coded???
  switch (whichBandOption)
  {
  case 0:
    freqCount = static_cast<float>(data.LOWEND40M) / 1000000.0;
    freqEnd = static_cast<float>(data.HIGHEND40M) / 1000000.0;
    pip = 0.1;
    chunks = 3;
    xDotIncrement = 12;
    xIncrement = (XAXISEND - XAXISSTART) / chunks;
    break;

  case 1:
    freqCount = static_cast<float>(data.LOWEND30M) / 1000000.0;
    freqEnd = static_cast<float>(data.HIGHEND30M) / 1000000.0;
    pip = 0.02;
    chunks = 3;
    xDotIncrement = 20;
    break;

  case 2:
    freqCount = static_cast<float>(data.LOWEND20M) / 1000000.0;
    freqEnd = static_cast<float>(data.HIGHEND20M) / 1000000.0;
    pip = 0.10;
    chunks = 3;
    xDotIncrement = 12;
    xIncrement = (XAXISEND * (.3 / .35) - XAXISSTART) / chunks;
    break;
  }
  tcolor = ILI9341_YELLOW;
  bcolor = ILI9341_BLACK;
  tickCount = 4;
  tft.fillRect(0, 47, PIXELWIDTH, PIXELHEIGHT, ILI9341_BLACK);
  tft.drawLine(XAXISSTART, YAXISSTART, XAXISSTART, YAXISEND, ILI9341_DARKGREY);     // Solid Y axis Left side  OK 9-8-19
  tft.drawLine(XAXISSTART, YAXISEND + 3, XAXISEND, YAXISEND + 3, ILI9341_DARKGREY); // X axis  OK 9-8-19
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);                                  // Lable X axis   OK 9-8-19
  tft.setCursor((XAXISEND - XAXISSTART) / 2 - 12, YAXISEND + 20);
  tft.print(" MHz");
  yIncrement = (YAXISEND - YAXISSTART) / 3; // Spacing for graph tick marks
  yTick = YAXISSTART + 5;                   // on Y axis
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  // Draw Y axis features.
  for (i = YAXISSTART; i < YAXISEND; i += yIncrement, yTick += yIncrement)
  {
    for (k = XAXISSTART + 10; k < XAXISEND; k += xDotIncrement)
    {
      // Draw a solid red line at SWR = 2.0.
      if(i == 131) tft.drawLine(XAXISSTART, 131, XAXISEND, 131, ILI9341_RED);
      else tft.drawPixel(k, yTick, ILI9341_DARKGREY); // Horizontal dotted axis OK 9-8-19 ILI9341_DARKGREY
    }
    tft.setCursor(0, yTick - 1); // Print y axis SWR labels
    tft.print(tickCount--);
    tft.print(".0");
  }

  xIncrement = (XAXISEND - XAXISSTART) / chunks; // Spacing for graph tick marks
  xTick = XAXISSTART - 10;                       // on X axis
  // Draw X axis features.
  for (i = 0; i < chunks + 1; i++, xTick += xIncrement)
  {
    tft.setCursor(XAXISSTART - 25 + i * xIncrement, YAXISEND + 20);
    if (freqCount > freqEnd)
    {
      freqCount = freqEnd;
    }
    if (whichBandOption == 2)
    { // 20M
      if (freqCount < 14.15 || freqCount < 14.25 || freqCount < 14.31 || freqCount < 14.4)
      {
        if (freqCount > 14.35)
        {
          freqCount = 14.35;
          std::sprintf(buff, "%3.1f", freqCount);
        }
        else
        {
          std::sprintf(buff, "%3.1f", freqCount);
        }
        tft.print(buff); // Print 20M Frequency Labels OK 9-8-19
      }
    }
    else
    {
      tft.print(freqCount); // Print 20M Frequency Labels OK 9-8-19
    }
    freqCount += pip;
  }
  for (k = XAXISSTART; k <= XAXISEND; k += xIncrement / 2)
  { // Draw horizontal dotted grid lines
    for (i = YAXISSTART; i < YAXISEND; i += 9)
    {
      tft.drawPixel(k, i, ILI9341_DARKGREY); // Print Vertical Y dotted lines OK 9-8-19
    }
  }
}

/*****
  Purpose: Plot frequency given by user

  Argument list:
    int whichBandOption       the selected band

  Return value:
    int                       the menu selected
*****/
void GraphPlot::PlotNewStartingFrequency(int whichBandOption)
{
  int delta, tickCount;
  int x, y;
  long highEnd, lowEnd, midPoint;
  float freqCount;
  float HzPerPix;
  switch (whichBandOption)
  {
  case 0:
    freqCount = static_cast<float>(data.LOWEND40M) / 1000000.0;
    highEnd = data.HIGHEND40M;
    lowEnd = data.LOWEND40M;
    HzPerPix = static_cast<float>(highEnd - lowEnd) / float(XAXISEND - XAXISSTART);
    x = 25 + static_cast<float>(dds.currentFrequency - lowEnd) / HzPerPix;

    break;
  case 1:
    freqCount = static_cast<float>(data.LOWEND30M) / 1000000.0;
    highEnd = data.HIGHEND30M;
    lowEnd = data.LOWEND30M;
    HzPerPix = static_cast<float>(highEnd - lowEnd) / float(XAXISEND - XAXISSTART);
    x = 25 + static_cast<float>(dds.currentFrequency - lowEnd) / HzPerPix;
    break;
  case 2:
    freqCount = static_cast<float>(data.LOWEND20M) / 1000000.0;
    highEnd = data.HIGHEND20M;
    lowEnd = data.LOWEND20M;
    HzPerPix = static_cast<float>(highEnd - lowEnd) / float(XAXISEND - XAXISSTART);
    x = 25 + static_cast<float>(dds.currentFrequency - lowEnd) / HzPerPix;
    break;
  }

  tft.drawLine(xOld, YAXISSTART, xOld, YAXISEND, ILI9341_BLACK);
  tft.drawLine(x, YAXISSTART, x, YAXISEND, ILI9341_YELLOW); // Y axis
  xOld = x;
}

/*****
  Purpose: Displays the measured SWR from the AutoTuneSWR() method in DisplayManagement.

  Parameter list:
    int whichBandOption     // The band being used.
    float swr    Array of SWR data.
    long tempCurrentPosition  Array of positions (frequency axis data).

  Return value:
    void
*****/
void GraphPlot::PlotSWRValueNew(int whichBandOption, int iMax, std::vector<int32_t>& tempCurrentPosition, std::vector<float>& tempSWR, int32_t SWRMinPosition)
{
  float stepsPerPix;
  int pixPerSWRUnit;
  float HzPerStep;
  float HzPerPix;
  float currentFrequencyDiff;
  float plotFreq;
  long freqStart;
  long freqEnd;
  switch (whichBandOption)
  { // This should use Data object to get band limits???
  case 0:
    freqStart = data.LOWEND40M;
    freqEnd = data.HIGHEND40M;
    break;

  case 1:
    freqStart = data.LOWEND30M;
    freqEnd = data.HIGHEND30M;
    break;

  case 2:
    freqStart = data.LOWEND20M;
    freqEnd = data.HIGHEND20M;
    break;
  }
  // This for loop plots the data to the axes.  The data is in the array tempSWR[i].
  for (int i = 0; i < tempCurrentPosition.size(); i++)
  {
    if (tempCurrentPosition[i] > 0 and tempSWR[i] < 3)
    {
      HzPerStep = (freqEnd - freqStart) / static_cast<float>(data.workingData.bandLimitPositionCounts[whichBandOption][1] - data.workingData.bandLimitPositionCounts[whichBandOption][0]);
      currentFrequencyDiff = float(tempCurrentPosition[i] - SWRMinPosition) * HzPerStep;
      plotFreq = (dds.currentFrequency + currentFrequencyDiff);
      HzPerPix = static_cast<float>(freqEnd - freqStart) / float(XAXISEND - XAXISSTART);
      stepsPerPix = static_cast<float>(data.workingData.bandLimitPositionCounts[whichBandOption][1] - data.workingData.bandLimitPositionCounts[whichBandOption][0]) / (XAXISEND - XAXISSTART);
      pixPerSWRUnit =static_cast<float>(YAXISEND - YAXISSTART) / 3;
      int xposition = 27 + static_cast<float>(plotFreq - freqStart) / HzPerPix;
      int yposition = YAXISSTART + (4 - tempSWR[i]) * pixPerSWRUnit;
      tft.fillCircle(xposition, yposition, 1, ILI9341_YELLOW);
    }
  }
}