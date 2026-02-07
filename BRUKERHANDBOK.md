# Plantesensor Brukerhåndbok

**Velkommen til ditt intelligente planteovervåkningssystem!** Denne enheten sporer automatisk plantens vannbehov og varsler deg når det er på tide å vanne. Ingen apper, ingen WiFi - bare enkle LED-indikatorer.

---

## Innholdsfortegnelse
1. [Hva du har](#hva-du-har)
2. [Førstegangs oppsett](#førstegangs-oppsett)
3. [Daglig bruk](#daglig-bruk)
4. [Forstå LED-mønstre](#forstå-led-mønstre)
5. [Sjekke status](#sjekke-status)
6. [Feilsøking](#feilsøking)
7. [Batteri og vedlikehold](#batteri-og-vedlikehold)

---

## Hva du har

Din plantesensor inkluderer:
- Hovedsensorenhet med vektcelle
- LED-indikator (synlig fra toppen)
- To knapper:
  - **TØR-knapp** (venstre/pin 2)
  - **STATUS-knapp** (høyre/pin 3)
- Batterirom (18650 Li-ion celle anbefalt)
- USB-ladeport (TP4056 lademodul)

---

## Førstegangs oppsett

### Steg 1: Installer batteri
1. Sett inn fulladet 18650 Li-ion batteri (anbefalt: Samsung INR18650-35E Protected)
2. Pass på riktig polaritet (+ og -)
3. LED skal begynne å puste sakte (3 pulser) - dette betyr at den er klar til kalibrering

### Steg 2: Plasser planten din
1. Plasser sensoren på et stabilt, plant underlag
2. Sentrer plantepotten din på vektcelleplattformen
3. Sørg for at planten ikke vil bevege seg eller flytte på seg

### Steg 3: Kalibrer tørrvekt
**⚠️ VIKTIG: Gjør dette kun når jorden er TØRR!**

1. Vent til plantens jord er helt tørr (klar for vanning)
2. Trykk på **TØR-knappen** én gang
3. LED blinker 3 raske ganger (bekrefter at kalibreringen er lagret)
4. LED vil nå pulsere én gang hvert 2. minutt (trenger vann)

### Steg 4: Vann planten din
1. Vann planten din normalt med din foretrukne metode:
   - Toppvanning (rask eller langsom helling)
   - Bunnvanning
   - Flere vanningsøkter - alt fungerer!
2. **Ingen knappetrykk nødvendig!** Sensoren detekterer vanning automatisk
3. Etter at vanning er detektert og jorden har stabilisert seg, slår LED seg AV
4. Sensoren din er nå ferdig satt opp! ✅

---

## Daglig bruk

**Gode nyheter: Du trenger ikke gjøre noe!** Sensoren overvåker planten din automatisk.

### Normal drift

**Når planten har det bra:**
- LED forblir **AV** (planten har nok vann)
- Sensoren sjekker vekt én gang per dag (sparer batteri)
- Batteriet varer ~2,4 år før det må byttes

**Når planten trenger vann:**
- LED pulserer **én gang** hvert 2. minutt (en mild påminnelse)
- Sensoren sjekker vekt hvert 2. minutt (overvåker for vanning)
- Bare vann planten din normalt - sensoren detekterer det automatisk!
- LED slår seg av automatisk etter at vanning er detektert

**Langsiktig neglisjering-varsel:**
- Hvis planten trenger vann i 14+ dager: LED pulserer **to ganger** hvert 2. minutt
- Ekstra påminnelse i tilfelle du gikk glipp av tidligere varsler

### Hvordan vanningsdeteksjon fungerer

Sensoren er smart! Den:
- Sporer vekt over de siste 60 minuttene
- Detekterer når vekten øker betydelig (vanning!)
- Venter på at jorden stabiliserer seg og overflødig vann renner av
- Oppdaterer automatisk sine referansepunkter
- Ingen knappetrykk nødvendig fra deg!

---

## Forstå LED-mønstre

| Hva du ser | Hva det betyr | Hva du skal gjøre |
|------------|---------------|-------------------|
| **3 langsomme pust** (600ms hver) | Ikke kalibrert ennå | Trykk TØR-knapp når jorden er tørr |
| **AV** | Planten har det bra! | Ingenting - nyt! ☕ |
| **1 puls** hvert 2. min | Planten trenger vann | Vann planten din (hvilken som helst metode) |
| **2 pulser** hvert 2. min | Trenger vann i 14+ dager | Vann planten din snart! |
| **3 raske blink** | Knappetrykk bekreftet | Du trykket nettopp på en knapp |
| **3 langsomme pulser** (800ms) | Konfigurasjon slettet | Start på nytt - trykk TØR når jorden er tørr |
| **LED dimmet** (50% lysstyrke) | Lavt batteri (<10%) | Bytt/lad batteri snart (~3 måneder igjen) |

**TIPS:** Alle LED-mønstre bruker myk uttoning - aldri harde blink!

---

## Sjekke status

### Sjekk status (Alt-i-ett)
Trykk på **STATUS-knappen** (høyre knapp) én gang for å se komplett status:

**Steg 1 - Vannnivå:**
- LED toner opp for å vise nåværende vannprosent
- Sterkere = mer vann
- Svakere = mindre vann
- Holder i 3 sekunder

**Steg 2 - Batterinivå:**
- LED blinker deretter for å vise batteri:
  - **4 blink** = 80-100% batteri (utmerket)
  - **3 blink** = 50-80% batteri (bra)
  - **2 blink** = 30-50% batteri (ok)
  - **1 blink** = 10-30% batteri (lavt - vurder å bytte snart)
  - **Raske blink** = <10% batteri (bytt innen 1-3 måneder)

**Ett knappetrykk = komplett status! 📊**

---

## Feilsøking

### LED puster konstant (3 langsomme pulser)
**Problem:** Ikke kalibrert ennå

**Løsning:**
1. Vent til jorden er helt TØRR
2. Trykk TØR-knapp én gang
3. Vann planten normalt når det trengs

---

### LED slår seg ikke av etter vanning
**Problem:** Sensoren har ikke detektert stabil vekt ennå

**Løsning:**
- Vent 10-30 minutter etter vanning for at overflødig vann skal renne av
- Sørg for at planten ikke blir flyttet eller støtt til
- Sjekk at vektcellen er på stabilt underlag
- Hvis den fortsatt ikke fungerer etter 2 timer, trenger planten kanskje mer vann

---

### LED kommer aldri på (selv når planten er tørr)
**Problem:** Våtvekt-referanse ikke satt ennå

**Løsning:**
- Dette skjer hvis du kalibrerte tørr, men aldri vannet nok
- Vann planten grundig (til du ser drenering)
- Vent på at drenering stopper (~30 min)
- Sensoren vil auto-detektere og lære våtvekt
- LED vil slå seg av når våtvekt er lært

---

### Knappene responderer ikke
**Problem:** Knappetilkobling eller strømproblem

**Løsning:**
1. Sjekk at batteriet er riktig installert
2. Prøv å trykke fast og hold i 1 sekund
3. Hvis batterinivå vises, men TØR-knappen ikke fungerer, fungerer sensoren - bare trykk hardere
4. Sjekk for 50ms debounce (ikke trykk for raskt)

---

### Ønsker å starte på nytt / rekalibrere
**Problem:** Du flyttet planten eller byttet potter

**Løsning:**
1. Trykk og hold **BEGGE knappene** i 2 sekunder
2. LED blinker 3 langsomme pulser (bekrefter tilbakestilling)
3. Start fra Steg 3 i [Førstegangs oppsett](#steg-3-kalibrer-tørrvekt)

---

## Batteri og vedlikehold

### Batterilevetid
- **Normal drift:** ~2,4 år på 3500mAh batteri
- **I tørr modus:** ~2,4 år (enheten sjekker oftere, men fortsatt effektivt!)
- **I OK modus:** 52+ år (teoretisk, sjekker én gang per dag)

### Når skal batteriet byttes
- LED dimmes automatisk til 50% når batteri <10%
- Dette gir deg ~3 måneders varsel
- STATUS-knappen viser batterinivå (se [Sjekke status](#sjekk-batterinivå))

### Anbefalt batteri
- **Samsung INR18650-35E (Protected)** - 3500mAh
- Må være "protected"-versjon (har innebygd sikkerhetskrets)
- Button-top foretrekkes for bedre kontakt
- Standard Li-ion kjemi (3,0-4,2V)

### Lading
- Plugg USB-kabel i ladeporten
- Rød LED på lader: Lader
- Grønn LED på lader: Fulladet
- Kan la planten stå på sensoren mens du lader (ingen avbrudd!)

### Strømoptimalisering
For maksimal batterilevetid bør Arduino Pro Minis strøm-LED fjernes (loddes av). Dette sparer ~2mA og er kritisk for å oppnå 2+ års batterilevetid. Hvis den ikke fjernes, forvent ~6 måneders batterilevetid i stedet.

### Rengjøring
- Tørk av vektcelleplattformen med fuktig klut
- Ikke senk sensoren i vann
- Hold elektronikk tørr
- Rengjør regelmessig for å forhindre jordopphopning som påvirker nøyaktighet

### Plantevekst
- Sensoren kompenserer automatisk for plantevekst!
- Etter hvert som planten vokser tyngre, oppdaterer sensoren sine referanser
- Ingen rekalibrering nødvendig over tid
- Maks vekstkompensasjon: 200g om gangen

---

## Tekniske spesifikasjoner

| Funksjon | Spesifikasjon |
|----------|---------------|
| **Strøm i dvale** | ~5 µA (ultra-lavt strømforbruk) |
| **Målestrøm** | ~15 mA i 2-3 sekunder |
| **Batterilevetid** | ~2,4 år (3500mAh) |
| **Sampling (Tørr)** | Hvert 2. minutt |
| **Sampling (OK)** | Hver 24. time |
| **Vektcellekapasitet** | 1-5kg (avhenger av modell) |
| **Vektnøyaktighet** | ±5g (stabile målinger) |
| **Vanningsdeteksjon** | 60-minutters rullerende buffer |
| **Driftsspenning** | 3,0-4,2V (Li-ion) |
| **Lavt batteri terskel** | 3,3V (~10% gjenstående) |

---

## Tips for best resultat

✅ **GJØR:**
- Kalibrer TØR når jorden faktisk er knusktørr
- Bruk stabilt, plant underlag for sensoren
- Hold planten sentrert på vektcellen
- Vann normalt - sensoren håndterer hvilken som helst metode!
- Fjern Arduino strøm-LED for lang batterilevetid

❌ **IKKE GJØR:**
- Kalibrer tørr på våt/fuktig jord
- Flytt planten rundt på sensoren
- Støt til eller ryst sensoren
- Bekymre deg for vanningsmetode (alle fungerer!)
- Forvent øyeblikkelige LED-oppdateringer (venter på stabilitet)

---

## Trenger du hjelp?

Hvis du opplever problemer som ikke er dekket i denne håndboken:
1. Prøv å slette konfigurasjon (begge knapper, 2 sekunder)
2. Rekalibrere fra bunnen av
3. Sjekk batterinivå
4. Verifiser at vektcellen er på stabilt underlag
5. Sørg for at planten ikke blir flyttet eller vibrert

Fortsatt fast? Sjekk den tekniske README.md eller opprett et issue på GitHub!

---

**Kos deg med din vedlikeholdsfrie plantepleie! 🌱**

Din plantesensor jobber stille i bakgrunnen og passer på planten din slik at du ikke trenger å gjøre det. Når LED-en pulserer, vann planten din. Når den er av, slapp av!
