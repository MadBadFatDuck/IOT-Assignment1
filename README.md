# Turn on the Sequence!

Un semplice gioco interattivo sviluppato per sistemi embedded a microcontrollore (es. Arduino).  
L’obiettivo è riprodurre correttamente una sequenza di LED accendendoli nell’ordine mostrato sul display.

## Componenti Hardware

- 4 LED verdi (**L1, L2, L3, L4**)  
- 1 LED rosso (**LS**)  
- 4 pulsanti tattili (**B1, B2, B3, B4**)  
- 1 potenziometro (impostazione difficoltà)  
- 1 display LCD (per mostrare la sequenza)  
- Microcontrollore compatibile (es. Arduino Uno o simile)

## Funzionamento

### Stato iniziale
- Tutti i LED verdi sono **spenti**.  
- Il LED rosso **LS** pulsa (effetto fade in/out) in attesa che il giocatore inizi la partita.  
- Se entro **10 secondi** non viene premuto **B1**, il sistema entra in **deep sleep** per risparmiare energia.  
- Il risveglio avviene premendo **qualsiasi pulsante**, ritornando allo stato iniziale con LS che pulsa di nuovo.

### Avvio del gioco
- Premendo **B1**, il gioco inizia.  
- Sul display LCD viene mostrata una **sequenza casuale di 4 cifre distinte**, ognuna compresa tra 1 e 4.  
- Ogni cifra rappresenta il numero del LED da accendere nell’ordine corretto tramite il corrispondente pulsante (B1–B4).

### Regole di gioco
- Il giocatore deve accendere i LED **nell’esatto ordine** della sequenza mostrata.  
- Se riesce a completare la sequenza nel **tempo limite**, il gioco prosegue con un livello di velocità maggiore.  
- Il tempo disponibile viene ridotto di un **fattore F**.  
- Se l’ordine è errato o il tempo scade:
  - Il LED rosso **LS** resta acceso per **2 secondi**.  
  - La partita termina e si ritorna allo stato iniziale.

### Livelli di difficoltà
Il potenziometro consente di scegliere il **livello di difficoltà** prima dell’avvio
Maggiore è la difficoltà, **più alto sarà il valore del fattore F** e quindi **più breve sarà il tempo concesso** per completare la sequenza.


## Fine
Progetto didattico per sistemi embedded — *“Turn on the Sequence!”*  
Realizzato come esercizio di programmazione e gestione input/output in ambiente microcontrollore.
[Assignment 1](https://github.com/pslab-unibo/esiot-2025-2026/blob/afeac2a3f7714b4b032c6892006c09e490535b7c/docs/assignment-01.md)


