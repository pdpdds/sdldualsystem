//      (_||_/
//      (    )       Classe Jeux
//     ( o  0 )
//-OOO?-(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 01/02/2006
//   (_)      (_/

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 or version 3 of the License.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef _JEUX_DOM_
#define _JEUX_DOM_

#include "preference.h"
#include "loco.h"

/******************************/
class Jeux {
 public:
  Jeux(void);
  ~Jeux(void);

  /*** Fonctions ***/
  /*****************/
  eMenu SDLMain(void);  
  bool Load(int NiveauN);
  bool DrawLevel(int NiveauN);   
  void PrendTouche(int Touche); 

  void TourneFleche(void); 
  void BufTouche(int Touche);
  int TestFleche(int Haut,int Bas,int Gauche,int Droite); 
  void AfficheEcran(void);    

  /*** Variables ***/
  bool Pause; // Si en pose

  long DureeJeu; // Horloge de dur? du jeu 

  int Key;       
  int Touche[4]; 
  int T[LT*HT]; 

  Loco Lo;      // G?e la locomotive

  int NumSS;
  int NumVideo; 
  bool Help;
};
#endif
