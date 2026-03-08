# GDL90 SafeSky Integration - Work in Progress

## Stato attuale (8 marzo 2026)

### Branch
- **Branch attivo**: `feature/gdl90-clean`
- **Ultimo commit**: `7cd4e2d85` - "feat: add GDL90 parser base integration and traffic report handling"
- **Upstream**: `origin/feature/gdl90-clean` (pushed)

### File modificati

#### Creati
- `Common/Source/Devices/GDL90/GDL90Protocol.h` - Header parser GDL90
- `Common/Source/Devices/GDL90/GDL90Protocol.cpp` - Implementazione parser

#### Modificati
- `Common/Source/Comm/DeviceDescriptor.h` - Aggiunto include + membri gdl90Parser
- `Common/Source/Comm/DeviceDescriptor.cpp` - Gestione memoria + metodo ParseGDL90()
- `Makefile` - Aggiunto GDL90Protocol.cpp alla compilazione (riga 1091)

### Cosa funziona ✅

1. **Parser GDL90 base**
   - Riconosce Message ID `0x14` (Traffic Report)
   - Riconosce Message ID `0x0A` (Ownship Report - stub)
   - Decodifica campi Traffic Report:
     * ICAO address (RadioId)
     * Latitudine/Longitudine (24-bit signed)
     * Altitudine (12-bit, offset -1000ft, step 25ft)

2. **Integrazione FLARM_Traffic**
   - Trova slot libero in `GPS_INFO.FLARM_Traffic[]`
   - Aggiorna campi:
     * `RadioId` (ICAO)
     * `Latitude`, `Longitude`
     * `Altitude` (convertita ft→m)
     * `Status = LKT_REAL`
     * `Time_Fix`

3. **Build**
   - Compila senza errori
   - Link corretto
   - Simboli presenti in `Bin/LINUX/Devices/GDL90/GDL90Protocol.o`

### Cosa manca ❌

1. **Ricezione dati**
   - `ParseGDL90()` non è mai chiamata dal flusso RX
   - Manca integrazione con:
     * Device driver (seriale/UDP/TCP)
     * Deframing GDL90 (byte `0x7E` start/stop, escape `0x7D`)
     * CRC verification

2. **Protocollo SafeSky**
   - **Non è chiaro** come SafeSky trasmette (UDP? TCP? porta?)
   - **Non è chiaro** se usa GDL90 puro o wrapped in altro protocollo
   - Serve documentazione SafeSky o sniff traffico di rete

3. **Parsing incompleto**
   - Traffic Report: mancano campi:
     * Track/Heading
     * Velocità orizzontale/verticale
     * Emitter category
     * Call sign
   - Ownship Report: stub (non implementato)
   - Altri message ID GDL90 non gestiti

4. **Testing**
   - Mai testato in volo
   - Mai testato con SafeSky reale
   - Nessun test di ricezione dati

### Prossimi passi (quando riprendi)

#### 1. Capire trasporto SafeSky
```bash
# Mentre SafeSky è attivo, sniffa traffico
ss -lunpt | grep -Ei "2000|4000|4353|49002"
sudo tcpdump -i wlan0 -n udp -vv -X
sudo tcpdump -i wlan0 -n tcp -vv -X


Cerca:

Porta UDP/TCP usata
Formato payload (ASCII NMEA vs binario GDL90)
Presenza byte 0x7E (flag GDL90)
2. Implementare ricezione
Se UDP: aggiungere socket listener in Comm/
Se TCP: modificare device driver esistente
Collegare flusso RX a ParseGDL90()
3. Deframing GDL90

Aggiungere in GDL90Protocol.cpp:


// Cerca 0x7E (flag), gestisci escape 0x7D, verifica CRCbool GDL90Parser::ProcessBuffer(const uint8_t* buffer, size_t length);
4. Completare parsing
Traffic Report: tutti i campi (velocità, track, emitter category)
Ownship Report: aggiornare GPS_INFO con pos/alt propria
CRC-16 CCITT verification
5. Testing
Test con SafeSky reale in volo
Verifica visualizzazione traffico su mappa LK8000
Log/debug per verificare ricezione messaggi
Risorse
Specifiche GDL90
Document: GDL 90 Data Interface Specification (FAA-E-2973b)
Message 0x14: Traffic Report - 28 bytes
Message 0x0A: Ownship Report - 28 bytes
Framing: Start/Stop 0x7E, Escape 0x7D
CRC: CRC-16 CCITT (poly 0x1021)
SafeSky
Documentazione: [da cercare/linkare]
Modalità output: GDL90? NMEA? Proprietario?
Porta/Protocollo: UDP 4000? TCP? Altro?
Comandi utili

# Riattiva branch
git checkout feature/gdl90-clean

# Status
git status -sb
git log --oneline -5

# Build
make clean
make -j$(nproc) TARGET=LINUX

# Verifica simboli
nm -C Bin/LINUX/Devices/GDL90/GDL90Protocol.o | grep "GDL90Parser::"

# Cerca chiamate a ParseGDL90
grep -RIn "ParseGDL90(" Common/Source

# Sniff rete (quando SafeSky attivo)
ss -lunpt
sudo tcpdump -i wlan0 -n udp port 4000 -vv -X


Note
Non aprire PR finché non testato in volo
Parser è base funzionante ma non production-ready
Manca completamente il "wire" per ricevere dati
Codice compila ma non fa nulla in runtime (nessuno chiama ParseGDL90)
Ultima modifica: 8 marzo 2026
Prossima sessione: verificare come SafeSky trasmette dati