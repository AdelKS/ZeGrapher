# Documentazione

![La finestra di ZeGrapher](images/overview.png)

- [1. Il grafico](#1-il-grafico)
  - [1.1. Spostare e zoomare](#11-spostare-e-zoomare)
- [2. Il pannello di inserimento](#2-il-pannello-di-inserimento)
  - [2.1. Estremi della vista](#21-estremi-della-vista)
  - [2.2. File](#22-file)
- [3. La scheda Matematica](#3-la-scheda-matematica)
  - [3.1. Aggiungere un oggetto](#31-aggiungere-un-oggetto)
  - [3.2. Funzioni e successioni](#32-funzioni-e-successioni)
  - [3.3. Costanti](#33-costanti)
    - [3.3.1. Animazione](#331-animazione)
    - [3.3.2. Più valori insieme](#332-più-valori-insieme)
  - [3.4. Equazioni parametriche](#34-equazioni-parametriche)
  - [3.5. Dati](#35-dati)
    - [3.5.1. La tabella dei dati](#351-la-tabella-dei-dati)
    - [3.5.2. Riempire una colonna da un oggetto](#352-riempire-una-colonna-da-un-oggetto)
    - [3.5.3. Importare un file CSV](#353-importare-un-file-csv)
  - [3.6. Stile del tratto](#36-stile-del-tratto)
- [4. La scheda Griglia — tacche e griglia](#4-la-scheda-griglia--tacche-e-griglia)
- [5. La scheda Grafico — aspetto e precisione](#5-la-scheda-grafico--aspetto-e-precisione)
- [6. La scheda App](#6-la-scheda-app)

## 1. Il grafico

![Il grafico, disegnato dagli oggetti della scheda Matematica](images/graph.png)

Il grafico è quello che vedi a destra, e quello che esporti. Le schede
[Griglia](#4-la-scheda-griglia--tacche-e-griglia) e
[Grafico](#5-la-scheda-grafico--aspetto-e-precisione) ne fissano l'aspetto, la
precisione e la dimensione.

### 1.1. Spostare e zoomare

Il grafico si comanda col mouse:

| Azione | Mouse |
|--------|-------|
| Spostare la vista | Trascinare col tasto sinistro |
| Zoomare i due assi insieme | Rotellina |
| Zoomare solo l'asse y | `Ctrl` + scorrimento verticale |
| Zoomare solo l'asse x | `Ctrl` + scorrimento orizzontale, oppure <br/> `Ctrl` + `Maiusc` + scorrimento verticale |

Con lo zoom, il punto sotto il cursore resta dov'è.

## 2. Il pannello di inserimento

![Il pannello, e i due pulsanti sul suo bordo](images/panel.png)

I quattro campi in cima al pannello sono gli estremi della vista. Sotto ci sono
quattro schede:

| Scheda | Cosa contiene |
|--------|---------------|
| **Matematica** | gli oggetti che tracci: equazioni, costanti, equazioni parametriche, dati |
| **Griglia** | tacche, griglia e sottogriglia |
| **Grafico** | dimensioni, carattere, sfondo, assi |
| **App** | lingua, carattere, colori della sintassi, aggiornamenti |

I tre pulsanti in basso a sinistra riguardano i [file](#22-file).

La freccia sul bordo del pannello lo chiude e lo riapre. La linea lungo il bordo
destro ne regola la larghezza.

Il pulsante a segnalibro, sotto quella freccia, apre questa documentazione e la
richiude.

### 2.1. Estremi della vista

I quattro campi in cima al pannello accettano espressioni, non solo numeri. Ogni
minimo deve restare sotto il suo massimo, e i campi rifiutano qualsiasi altro
valore.

![Gli estremi della vista](images/range.png)

### 2.2. File

![I pulsanti di esportazione, salvataggio e apertura](images/files.png)

I tre pulsanti in basso a sinistra nel pannello sono, in ordine:

1. **Esporta il grafico** che vedi, in formato vettoriale (`svg`, `pdf`) o come
   immagine (`png`, `jpeg`, `bmp`, `ppm`). Il file esce identico al grafico
   sullo schermo.
2. **Salva** tutto — oggetti, dati, vista, impostazioni — in un documento
   ZeGrapher (`.zg`).
3. **Apri** un documento di questo tipo.

Al prossimo avvio, ZeGrapher riapre il tuo ultimo lavoro. Se passi un file `.zg`
sulla riga di comando, apre quel file al suo posto.

## 3. La scheda Matematica

![La scheda Matematica, con una costante e due equazioni](images/math-tab.png)

Questa scheda definisce gli oggetti da tracciare, o da usare dentro altri
oggetti: funzioni, successioni, costanti (che non si tracciano mai), equazioni
parametriche e colonne di dati.

### 3.1. Aggiungere un oggetto

Questo pulsante, in fondo alla scheda Matematica, aggiunge un oggetto.

![Il pulsante che aggiunge un oggetto](images/add-object.png)

L'elenco in cima alla nuova scheda sceglie il tipo di oggetto.

![Un oggetto e la sua scheda](images/object-card.png)

Ogni scheda di oggetto porta gli stessi pulsanti:

- L'occhio mostra o nasconde la curva.
- Il pennello apre lo [stile del tratto](#36-stile-del-tratto).
- Il disco è il colore della curva.
- La maniglia a destra riordina gli oggetti quando la trascini.
- La **×** nell'angolo elimina l'oggetto.

### 3.2. Funzioni e successioni

Una funzione si definisce con la sua equazione naturale:

```
f(x) = 2 + cos(x)
```

Queste funzioni sono già pronte, e valgono in ogni espressione:

| Tipo | Funzioni |
|------|----------|
| Trigonometria | `cos`, `sin`, `tan`, `acos`, `asin`, `atan` |
| Iperboliche | `cosh`, `sinh`, `tanh`, `acosh`, `asinh`, `atanh` |
| Iperboliche, nomi brevi | `ch`, `sh`, `th`, `ach`, `ash`, `ath` |
| Potenze e logaritmi | `sqrt`, `exp`, `ln` (base e), `log` (base 10), `lg` (base 2) |
| Arrotondamento | `floor`, `ceil` |
| A due argomenti | `max`, `min` |
| Altre | `abs`, `erf`, `erfc`, `gamma` (si scrive anche `Γ`) |

Queste costanti ci sono già:

| Nome | Valore |
|------|--------|
| `math::pi`, `math::π` | 3.141592653589793 |
| `physics::kB` | la costante di Boltzmann, 1.380649e-23 |
| `physics::h` | la costante di Planck, 6.62607015e-34 |
| `physics::c` | la velocità della luce nel vuoto, 299792458 |

Le tre costanti fisiche sono in unità del SI.

Una successione è un elenco di espressioni separate da `,` o `;`. Le prime
espressioni sono i primi termini. L'**ultima** è il termine generale, e vale per
tutti gli indici che seguono.

```
u(n) = 0 ; 1 ; 0.5*(u(n-2) + u(n-1))
```

![Una funzione e una successione](images/functions.png)

Il bordo di un campo prende un colore che dice se l'espressione è valida. Quando
non lo è, sotto compare il motivo.

![Un campo vuoto, uno valido e uno non valido](images/input-states.png)

Un campo che aspetta un valore, come un estremo della vista, può prendere invece
il colore di avviso: l'espressione è valida, ma non dà nessun numero.

![Un estremo la cui espressione non ha valore](images/input-warning.png)

Questi tre colori li scegli nella [scheda App](#6-la-scheda-app).

### 3.3. Costanti

Una **costante** è un nome con un valore numerico. Ogni altro oggetto, tranne
un'altra costante, può poi usarla nella propria espressione.

![Una costante, il suo cursore e la sua animazione](images/constant.png)

#### 3.3.1. Animazione

Il cursore sotto porta il valore da **min** a **max**, e tutte le curve che usano
la costante lo seguono. Trascina il cursore a mano, oppure lascialo andare. La
riga sotto il cursore ha i comandi dell'animazione: avvio, ciclo, avanti e
indietro, e la durata di un passaggio.

#### 3.3.2. Più valori insieme

Questo pulsante, su una scheda di costante, la rende una **costante di
Schrödinger**.

![Il pulsante col gatto, acceso](images/cat-button.png)

La costante prende allora `passi + 1` valori, a distanze uguali da min a max.
Ogni oggetto che la usa viene tracciato una volta per valore.

![Una costante che prende sei valori insieme](images/schrodinger.png)

Questi oggetti ricevono un secondo disco di colore. La famiglia di curve viene
disegnata in sfumatura, dal primo colore al secondo.

### 3.4. Equazioni parametriche

Un'equazione parametrica è una coppia di oggetti che dà le coordinate di ogni
punto della curva. I loro nomi si scrivono nei due campi:

![Un'equazione parametrica su due funzioni](images/parametric.png)

La coppia viene tracciata fra l'**Inizio** e la **Fine**, definiti nello
[stile del tratto](#36-stile-del-tratto) dell'equazione parametrica.

### 3.5. Dati

![Un foglio di dati, la sua tabella e il suo tratto](images/data.png)

Un oggetto di dati è un foglio di colonne con un nome. Ogni colonna è un oggetto
matematico a sé. Ha un nome, un pulsante a occhio, uno stile del tratto, un
colore, una maniglia per riordinarla, e una **×** nell'angolo che la elimina.

![Una colonna di un foglio](images/column-card.png)

I valori di una colonna vengono tracciati contro il loro indice: il primo valore
in x = 0, il secondo in x = 1, e così via. Per tracciare una colonna contro
un'altra, usa un'[equazione parametrica](#34-equazioni-parametriche).

![I pulsanti sotto un foglio](images/sheet-buttons.png)

I pulsanti in basso a destra del foglio importano un file CSV, scrivono le
colonne in un file CSV, e aggiungono una colonna. La barra sotto cambia
l'altezza del foglio. Fai doppio clic sulla barra per ridargli l'altezza
predefinita.

#### 3.5.1. La tabella dei dati

![Due colonne nella tabella](images/data-table.png)

Il pulsante **tabella** di una colonna la mostra nella tabella accanto al
pannello. Nella tabella:

- fai clic su una cella e scrivi per cambiarla, oppure premi `Invio` per aprire
  l'editor,
- fai clic su un'intestazione per selezionare tutta una riga o tutta una colonna,
- trascina per selezionare un rettangolo di celle,
- premi `Backspace` per svuotare le celle scelte, e `Canc` per eliminarle,
- il menu del tasto destro ha le stesse due azioni, sotto *Svuota* ed *Elimina*,
  e in più *Inserisci riga sopra* e *Inserisci riga sotto*. Le due inserzioni
  agiscono sulla cella attiva.

Se elimini una colonna intera, sparisce anche l'oggetto colonna.

#### 3.5.2. Riempire una colonna da un oggetto

Il pulsante a istogramma di una colonna apre un piccolo modulo: dai il nome di
un oggetto, e un **Inizio**, una **Fine** e un **Passo**. Il pulsante di conferma
preleva i valori dell'oggetto e li scrive nella colonna.

![Riempire una colonna con i valori di f](images/sampling.png)

#### 3.5.3. Importare un file CSV

Il pulsante CSV sta su un foglio, oppure in fondo alla scheda Matematica per
creare un foglio nuovo.

![Il pannello di importazione CSV](images/csv-import.png)

Apre una finestra di scelta file. Poi un pannello laterale mostra un'anteprima
del file e le opzioni con cui viene letto. L'anteprima segue ogni cambiamento:

- Indica il separatore (scrivi `\t` per la tabulazione).
- Indica quante righe saltare all'inizio del file (commenti o parametri, per
  esempio).
- Segna se la prima riga porta i nomi delle colonne.
- **Righe da mostrare** è quante righe si vedono nell'anteprima.
- **Carica** crea le colonne da quello che c'è nell'anteprima.
- **Annulla** lascia tutto com'era.

Abbiamo provato ZeGrapher su file CSV da diversi milioni di celle.

### 3.6. Stile del tratto

Il pulsante col pennello apre le impostazioni che decidono come viene disegnato
un oggetto:

![Lo stile del tratto di un oggetto](images/style.png)

- Coordinate **cartesiane** o **polari**.
- Il tipo di linea — continua, tratteggiata, tratto e punto, punteggiata, o
  nessuna linea — e il suo spessore.
- Per gli oggetti disegnati a punti (successioni e dati), la forma e la
  dimensione dei punti.
- **Inizio** e **Fine**: l'intervallo su cui l'oggetto viene tracciato. Di
  partenza sono `xmin` e `xmax`, gli estremi della vista. Tutti e due i campi
  accettano qualsiasi espressione, per esempio `-math::pi` e `4*math::pi`.

## 4. La scheda Griglia — tacche e griglia

![La scheda Griglia](images/grid-tab.png)

**Coordinate** regola i numeri lungo gli assi: il loro passo, quante cifre
possono usare, e un **moltiplicatore**. Il moltiplicatore è un'espressione,
quindi le tacche possono essere multipli di `math::pi`, e le etichette si
scrivono come multipli di quel valore.

**Griglia** e **Sottogriglia** si regolano separatamente per x e per y. Ognuna ha
un colore, uno spessore di linea, e un interruttore che la mostra o la nasconde.
Alla sottogriglia dici anche in quante parti taglia ogni cella.

## 5. La scheda Grafico — aspetto e precisione

![La scheda Grafico](images/graph-tab.png)

Di partenza il grafico riempie la finestra. Metti **Dimensioni del grafico** su
*Personalizzate*, nel riquadro **Dimensioni** in cima, e il grafico diventa un
foglio della dimensione che indichi. Quella dimensione va in pixel, o in
**centimetri reali**. Un centimetro è un centimetro vero sullo schermo, e resta
tale in un `pdf` o in un `svg` esportato. **Scala**, nello
stesso riquadro, ingrandisce o riduce tutto il disegno con una sola
impostazione.

Questo foglio viene disegnato come una pagina dentro la finestra, con una barra
di zoom sopra:

![La barra di zoom](images/zoom-bar.png)

La barra ingrandisce o riduce il foglio sullo schermo, e accetta anche una
percentuale di zoom. L'ultimo pulsante fa entrare tutto il foglio nella finestra.
Questo zoom cambia solo la dimensione con cui il foglio viene disegnato. Gli
estremi della vista restano come sono.

I due riquadri sotto **Dimensioni**:

- **Disegno**: il **carattere** del grafico e il colore del suo **sfondo**.
  **Punti min** e **Punti max** sono quanti punti vengono calcolati per ogni
  curva continua, in potenze di due. Più punti danno un tratto più fine e un
  disegno più lento.
- **Assi**: spessore della linea, colore, i titoli scritti lungo x e y, e il
  carattere di quei titoli.

## 6. La scheda App

![La scheda App](images/app-tab.png)

Questa scheda ha la lingua e il carattere dell'interfaccia. Ha anche i tre colori
dei campi di inserimento: valida, avviso e non valida. L'ultimo pulsante chiede a
zegrapher.com se è uscita una versione più nuova.
