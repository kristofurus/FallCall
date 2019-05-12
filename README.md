# FallerCaller
Hackathon software

Kod do urządzenia FallerCaller które ma na celu sprawdzanie czy użytkownik
nie upadł a w przypadku takiego zdarzenia poinformowaniu osoby do której
wcześniej został podany e-mail.
Program pozwala na zmienianie wejscia przycisku i buzzera przy zmianie wartosci
jedynie definicji znajdującej się na początku kodu, a poza tym sprawdza akceleracje
oraz czy użytkownik kliknął odpowiednią ilość razy na przycisk w celu anulowania alarmu.

Do poprawnego działania potrzebne są biblioteki:
AlertMe.h https://github.com/connornishijima/AlertMe/blob/master/src/AlertMe.h
Oraz zbiór bibliotek do obsługi modemu WiFi które można pobrać z repozytorium
https://github.com/esp8266/Arduino a następnie poprzez przeniesienie wszystkich folderów
z folderu library do lokalenej biblioteki która w przypadku ArduinoIDE najczęściej znajduje się 
w documents/Arduino/library.
