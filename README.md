![YIAC0768](https://user-images.githubusercontent.com/45856645/113737284-91fff200-9706-11eb-9d06-084402961773.JPG)
![YIAC0788](https://user-images.githubusercontent.com/45856645/113737295-93c9b580-9706-11eb-8f9a-869be2d7e4f1.JPG)
![YIAC0789](https://user-images.githubusercontent.com/45856645/113737299-94624c00-9706-11eb-8bd2-0022e2730409.JPG)
![YIAC0796](https://user-images.githubusercontent.com/45856645/113737304-94fae280-9706-11eb-9f47-b7c12d86f159.JPG)
# TapTap

TapTap este un joc ce presupune prinderea notelor muzicale.
La pornirea jocului, ecranul LCD va afisa cateva optiuni, Play, High Score si Options. Cu ajutorul a 4 butoane putem naviga prin meniu.
Din Options putem seta dificultatea care poate fi Easy, Medium sau High, putem seta luminozitatea matricei si contrastul ecranului LCD.
HighScore va salva automat in EEPROM cel mai mare scor pozitiv.
Daca selectam Play , alegem una din 5 melodii si putem incepe jocul.
Jocul distribuie notele muzicale ale melodiei dupa cum urmeaza:
    - notele 1, 2 si 3 se vor afisa pe coloanele 1 si 2
    - notele 4 si 5 se vor afisa pe coloanele 4 si 5
    - notele 6 si 7 se vor afisa pe coloanele 7 si 8
Pentru fiecare nota muzicala matricea va aprinde o linie si doua coloane.
Ledurile aprinse vor parcurge matricea de sus in jos cu o viteza variabila in functie de dificultatea aleasa.
Pentru a ne juca avem la dispozitie o telecomanda cu 3 butoane. Fiecare buton corespunde unei perechi de coloane formate.
Cand notele ajung pe ultima linie trebuie sa apasam butonul corespunzator liniei. 
Regulile sunt urmatoarele: 
    - Daca apasam butonul cand avem nota pe ultima linie, castigam 50 de puncte.
    - Daca apasam butonul cand nu avem nota, pierderm 10 puncte.
    - Daca avem nota dar nu apasam butonul la timp, nota se considera pierduta. Un led rosu corespunzator liniei ne va semnala acest lucru       si pierdem 15 puncte.
    
In functie de dificultatea aleasa, muzica generata de note va fi mai lenta sau mai rapida iar notele afisate pe matrice vor cadea mai lent sau mai rapid.

Link: https://youtu.be/ylo7BzJz6hE
