# ESP32 IoT Meteostanica
**Meranie teploty a vlhkosti s online zobrazením údajov**

## Popis projektu
Projekt **ESP32 IoT Meteostanica** slúži na meranie teploty a vlhkosti vzduchu pomocou senzora DHT11.  
Namerané údaje sú spracované mikrokontrolérom `ESP32`, zobrazované na webovej stránke v lokálnej sieti a zároveň odosielané do cloudovej platformy ThingSpeak, kde sú vizualizované vo forme grafov.  
Projekt obsahuje aj vizuálny alarm signalizovaný LED diódou pri prekročení nastavených prahových hodnôt.

## Použitý hardvér
- ESP32 Dev Module  
- Senzor teploty a vlhkosti DHT11  
- LED dióda + rezistor  
- Breadboard  
- Prepojovacie vodiče  
- USB kábel  

## Použitý softvér a nástroje
- Arduino IDE  
- Programovací jazyk C++  
- Platforma ThingSpeak  
- Webový prehliadač  

## Funkcionalita projektu
- meranie teploty a vlhkosti v reálnom čase  
- zobrazenie aktuálnych hodnôt na webovej stránke ESP32  
- automatická aktualizácia údajov  
- vizuálny alarm pri prekročení nastavených hodnôt  
- odosielanie dát do cloudovej platformy ThingSpeak  
- zobrazenie historických údajov vo forme grafov  
- prístup k údajom cez lokálnu sieť (PC aj mobilné zariadenie)  

Alarm sa aktivuje pri prekročení teploty **23 °C** alebo vlhkosti **60 %**.

## Používateľské rozhranie (UI)
Používateľské rozhranie projektu je webová stránka v slovenskom jazyku, ktorá zobrazuje:
- aktuálnu teplotu  
- aktuálnu vlhkosť  
- stav alarmu  
- informácie o pripojení a chode zariadenia  

## Zapojenie
Zapojenie hardvéru je realizované na nepájivom poli (breadboard).  
Fotografie zapojenia a schéma zapojenia sa nachádzajú v priečinku **/dokumentácia**.

## Zdrojové kódy
Zdrojový kód pre ESP32 sa nachádza v priečinku **/zdroj**.  
Kód zabezpečuje:
- komunikáciu so senzorom DHT11  
- pripojenie na Wi-Fi sieť  
- prevádzku webového servera  
- odosielanie údajov na platformu ThingSpeak  

## Obrazová dokumentácia
V priečinku **/dokumentácia** sa nachádzajú:
- fotografie zapojenia projektu  
- snímky webového rozhrania  
- grafy z platformy ThingSpeak  
- poster projektu vo formáte A4  

## Praktické využitie
Projekt demonštruje praktické využitie technológií Internetu vecí (IoT) v oblasti monitorovania prostredia, napríklad v domácnosti, učebni alebo kancelárii.

## Zdroje
- Arduino dokumentácia  
- Dokumentácia ESP32  
- Dokumentácia ThingSpeak  
- Online návody a príklady pre ESP32 a DHT11

## Online vizualizácia dát (ThingSpeak)

Namerané údaje sú odosielané do platformy ThingSpeak, kde sú
verejne dostupné vo forme grafov:

🔗 https://thingspeak.com/channels/3216200
