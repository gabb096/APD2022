# VibLay VST2 plugin

### English

This project is a VST2 plugin consisting of a delay with rhythmic vibrato and distortion developed for the Audio Plugin Development exam.

Following is the block diagram, In cyan are the user-modifiable parameters:

![](Schematic.png)

##### Signal Path
The incoming signal has two paths ahead of it, the first leading directly to a `Dry/Wet` control to mix the direct signal from the effected signal.
The second path takes the signal into the `DELAY` section where it will be summed with a delayed copy of it and multiplied by the `Feedback` coefficient.

Next we find the `VIBRATO` stage. 
Through the `Var[x]` parameter the user can select the type of amplitude modulation, from classic sine modulation to those with Eudlidean rhythms. 
The `Rate` parameter allows the user to change the speed at which the modulation takes place.
The preset selector can be used to enable and disable the `Ping Pong` effect.

Finally we find the `SATURATION` stage, where the signal is first saturated with an approximation of the hyperbolic tangent, distorted with a slight `WaveFolding`, and finally filtered with a low-pass, before arriving at the `Dry/Wet` control.

---

### Italiano

Questo progetto è un plugin VST2 che consiste in un delay con vibrato ritmico e distorsione, sviluppato per l'esame Audio Plugin Development.

Di seguito è riportato lo schema a blocchi, in ciano i parametri modificabili dall'utente:

![](Schematic.png)

##### Percorso del Segnale

Il segnale in ingresso trova davanti a se due strade, la prima porta direttamente ad un controllo di `Dry/Wet` per miscelare il segnale diretto da quello effettato.
La seconda strada porta il segnale nella sezione `DELAY` dove verrà sommato con una sua copia ritardata e moltiplicata per il coefficiente di `Feedback`.

Successivamente troviamo lo stadio di `VIBRATO`. 
Tramite il parametro `Var[x]` l'utente può selezionare il tipo di modulazione in ampiezza, dalla classica modulazione sinusoidale a quelle con ritmiche Eudlidee. 
Il parametro `Rate` permette di modificare la velocità con cui avviene la modulazione.
Tramite il selettore dei preset è possibile abilitare e disabilitare l'effetto `Ping Pong`.

Infine troviamo lo stadio di `SATURAZIONE` dove il segnale viene prima saturato con una approssimazione della tangente iperbolica, distorto con un leggero `WaveFolding` e finalmente filtrato con un passa basso, prima di arrivare al controllo di `Dry/Wet`