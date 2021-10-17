/*
 *    Source from Retrobattle, a NES-like collect-em-up.
 *    Copyright (C) 2010 Andreas Remar
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    Original authors contact info: andreas.remar@gmail.com
 */

#include "ScoreSign.h"

ScoreSign::ScoreSign(remar2d *gfx, ScoreSign::ScoreSignAmount amount, int x, int y)
  : Object(gfx, "score sign", 0)
{
  destroyTimer = 60;

  const char *animation[] = {"200", "250", "300", "400"};
  setAnimation(animation[amount]);
  setVisible(true);
  moveAbs(x, y + 5);
}
