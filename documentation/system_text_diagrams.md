# Systembeskrivelse og tekstdiagrammer

Dette dokument er skrevet som tekstgrundlag til portfolioen. Diagrammerne er beskrevet i tekst, så de senere kan tegnes pænt i et valgfrit designværktøj.

## Gennemgang af iterationer

### Iteration 1

I første iteration blev grundspillet lavet som et terminalbaseret C++-program. Spilleren kan oprette en karakter, som starter med to monstre af typen `Hest`. Karakteren kan have op til fire monstre. Spilleren kan vælge et fjendtligt monster fra en liste og kæmpe mod det med et af sine egne levende monstre.

Kampsystemet er simpelt: der vælges tilfældigt hvem der starter, og derefter skiftes monstre til at angribe hinanden. Hvis spillerens monster vinder, kan det besejrede monster tilføjes til karakterens samling. Hvis karakteren allerede har fire monstre, kan spilleren vælge at udskifte et eksisterende monster.

Udviklingsflowet i iteration 1 var at oprette de centrale domæneklasser først (`Monster`, `Character`, `MonsterCatalog`), derefter kampklassen (`Battle`), input-hjælperen (`Input`) og til sidst selve spilflowet (`Game`). Der blev lavet separate commits for nye klasser.

### Iteration 2

I anden iteration blev spillet udvidet med `Ting`, `Status` og `Grotter`. Monstre kan nu have flere ting, og spilleren kan bruge en ting i stedet for et almindeligt angreb. Nogle ting giver direkte skade, mens andre kan give en status til modstanderen.

Statusser bliver afviklet i starten af et monsters tur. Eksempler er `Poisoned`, som giver skade over tid, `Frozen` og `Stunned`, som kan forhindre monsteret i at handle, `Paralyzed`, som giver chance for at miste turen, og `Cursed`, som giver skade efter angreb.

Grotter fungerer som en samling af fjendtlige monstre. Spilleren vælger en grotte, og alle grottens monstre skal besejres for at gennemføre den. Når en grotte gennemføres, får spilleren en tilfældig ting, som kan gives til et af karakterens monstre.

Udviklingsflowet i iteration 2 var at oprette de nye klasser (`StatusEffect`, `Item`, `Cave`) separat og derefter integrere dem i `Monster`, `Battle` og `Game`.

### Iteration 3

I tredje iteration blev der tilføjet databasefunktionalitet med SQLite3. Formålet er at kunne gemme og indlæse helte samt gemme statistik om, hvad der sker i spillet.

Spilleren kan nu se gemte helte ved opstart, loade en eksisterende helt, gemme den nuværende helt og se statistik. Databasen gemmer karakterens navn, monstre, monstrets ting og statistik. Statistikken omfatter blandt andet antal besejrede monstre i alt, hvor mange monstre der er besejret af hver ting, mest brugte ting og mest brugte monster for hver spiller.

Udviklingsflowet i iteration 3 var først at udvide systemet med statistikdata, derefter oprette en separat `Database`-klasse til al SQLite-kommunikation og til sidst koble save/load og statistik ind i hovedmenuen.

## Beskrivelse af det nuværende system

Det nuværende system er et objektorienteret C++ terminalspil. Spilleren styrer en karakter, der ejer en gruppe monstre. Monstre kan kæmpe mod fjender, bruge ting i kamp og blive påvirket af statusser. Spilleren kan enten kæmpe mod et enkelt monster eller gennemføre en grotte med flere monstre.

Systemet består af følgende hoveddele:

- `Game` styrer hovedmenuen og det overordnede spilflow.
- `Input` håndterer valideret terminalinput.
- `Character` repræsenterer spillerens helt og ejer spillerens monstre.
- `Monster` repræsenterer både spillerens og fjendens monstre.
- `Item` repræsenterer ting, der kan bruges i kamp.
- `StatusEffect` repræsenterer midlertidige effekter i kamp.
- `Cave` repræsenterer en grotte med flere fjendtlige monstre.
- `Battle` afvikler kampe mellem monstre.
- `MonsterCatalog` indeholder standardlisten af monstre.
- `Database` gemmer og indlæser helte og statistik via SQLite3.

Programmet gemmer data i en lokal SQLite-databasefil. Databasen er skjult bag `Database`-klassen, så resten af systemet ikke behøver at kende SQL-detaljerne.

## Use Cases

### Use Case-oversigt

Tekstdiagram:

```text
Spiller
  -> Lav ny karakter
  -> Load gemt helt
  -> Gem nuværende helt
  -> Vis karakter
  -> Start eventyr
       -> Kæmp mod enkelt monster
       -> Kæmp mod grotte
            -> Besejr alle grottens monstre
            -> Modtag ting
       -> Vælg eget monster
       -> Angrib
       -> Brug ting
  -> Vis statistik
  -> Luk spillet
```

### Use Case-beskrivelser

**Lav ny karakter**

Spilleren indtaster et navn. Systemet opretter en ny karakter med to startmonstre af typen `Hest`.

**Load gemt helt**

Systemet viser alle gemte helte fra databasen. Spilleren vælger en helt, og systemet indlæser heltens monstre, ting og statistik.

**Gem nuværende helt**

Systemet gemmer den aktive helt i SQLite-databasen. Hvis helten allerede findes, overskrives de gamle data med den aktuelle tilstand.

**Start eventyr**

Spilleren kan vælge mellem at kæmpe mod et enkelt monster eller en grotte.

**Kæmp mod enkelt monster**

Spilleren vælger en fjende fra monsterlisten og vælger derefter et af sine egne levende monstre. Kampen afvikles i `Battle`.

**Kæmp mod grotte**

Systemet genererer flere grotter, som passer til spillerens nuværende niveau. Spilleren vælger en grotte og skal besejre alle grottens monstre. Når grotten gennemføres, modtager spilleren en ting.

**Brug ting**

Under kamp kan spilleren vælge en ting i stedet for et normalt angreb. Tingen kan give skade eller status til fjenden.

**Vis statistik**

Systemet læser statistik fra databasen og viser blandt andet total antal besejrede monstre, besejrede monstre per ting, mest brugte ting og mest brugte monster.

## Domain Model

Tekstdiagram:

```text
Character
  - name
  - monsters
  - stats

Monster
  - name
  - hitPoints
  - maxHitPoints
  - strength
  - items
  - statuses

Item
  - name
  - damage
  - statusChance
  - statusType

StatusEffect
  - type
  - remainingTurns

Cave
  - name
  - monsters

GameStats
  - totalDefeatedMonsters
  - itemUses
  - itemDefeats
  - monsterUses

Database
  - gemmer og indlæser Character
  - gemmer og læser GameStats
```

Relationer:

```text
Character 1 --- 0..4 Monster
Monster 1 --- 0..* Item
Monster 1 --- 0..* StatusEffect
Cave 1 --- 1..* Monster
Character 1 --- 1 GameStats
Game bruger Database
Game bruger Battle
Game bruger MonsterCatalog
Battle bruger Monster
Database gemmer/loader Character og GameStats
```

## Udvalgte sekvensdiagrammer

### Sekvens: Opret ny karakter

```text
Spiller -> Game: Vælg "Lav ny karakter"
Game -> Input: Bed om navn
Input -> Game: Returner navn
Game -> Character: Opret Character(name)
Game -> MonsterCatalog: Hent starter Hest
Game -> Character: Tilføj Hest
Game -> Character: Tilføj Hest
Game -> Spiller: Vis at karakteren er oprettet
```

### Sekvens: Kamp hvor spilleren bruger en ting

```text
Spiller -> Game: Start eventyr
Game -> Spiller: Vælg fjende/grotte
Spiller -> Game: Vælg fjende
Game -> Spiller: Vælg eget monster
Spiller -> Game: Vælg monster
Game -> Battle: fight(playerMonster, enemyMonster)
Battle -> Monster: applyStartOfTurn()
Battle -> Spiller: Vis handlinger
Spiller -> Battle: Vælg ting
Battle -> Item: use(playerMonster, enemyMonster)
Item -> Monster: takeDamage() eller addStatus()
Battle -> Game: Returner BattleResult
Game -> Character.stats: Opdater itemUses, monsterUses og evt. itemDefeats
Game -> Spiller: Vis kampresultat
```

### Sekvens: Gennemfør grotte

```text
Spiller -> Game: Vælg "Kæmp mod en grotte"
Game -> Cave: Generer grotter ud fra karakter
Game -> Spiller: Vis mulige grotter
Spiller -> Game: Vælg grotte
loop For hvert levende monster i grotten
    Game -> Battle: Start kamp mod næste grottemonster
    Battle -> Game: Returner resultat
end
Game -> Item: Vælg tilfældig belønning
Game -> Spiller: Vælg hvilket monster der får tingen
Game -> Monster: addItem(item)
```

### Sekvens: Gem helt

```text
Spiller -> Game: Vælg "Gem nuværende helt"
Game -> Database: saveCharacter(character)
Database -> SQLite: Find eller opret hero-række
Database -> SQLite: Gem monstre
Database -> SQLite: Gem monster_items
Database -> SQLite: Gem stats
Database -> SQLite: Gem item_uses, item_defeats, monster_uses
Database -> Game: Returner success/failure
Game -> Spiller: Vis besked
```

### Sekvens: Load helt

```text
Spiller -> Game: Vælg "Load gemt helt"
Game -> Database: listHeroes()
Database -> SQLite: SELECT gemte helte
Database -> Game: Returner helte
Game -> Spiller: Vis helte
Spiller -> Game: Vælg helt
Game -> Database: loadCharacter(heroId)
Database -> SQLite: Hent hero
Database -> SQLite: Hent monstre og ting
Database -> SQLite: Hent statistik
Database -> Game: Returner Character
Game -> Spiller: Vis at helten er indlæst
```

## UML - Class Diagram

Tekstdiagram:

```text
class Game
  + run()
  - showMainMenu()
  - createNewCharacter()
  - loadCharacterFromDatabase()
  - saveCharacterToDatabase()
  - showStatistics()
  - startAdventure()
  - fightEnemy()
  - startSingleMonsterFight()
  - startCaveFight()
  - offerCapturedMonster()
  - giveItemToMonster()

class Character
  + name()
  + monsters()
  + stats()
  + addMonster()
  + replaceMonster()
  + hasLivingMonsters()
  + livingMonsterIndexes()

class Monster
  + name()
  + hitPoints()
  + maxHitPoints()
  + strength()
  + items()
  + takeDamage()
  + healToFull()
  + addItem()
  + addStatus()
  + applyStartOfTurn()
  + applyAfterDealingDamage()
  + summary()

class Item
  + name()
  + dealsDamage()
  + summary()
  + use()
  + fromName()

class StatusEffect
  + type()
  + name()
  + remainingTurns()
  + applyStartOfTurn()
  + applyAfterDealingDamage()

class Cave
  + name()
  + monsters()
  + hasLivingMonsters()
  + livingMonsterIndexes()
  + generateForCharacter()

class Battle
  + fight()
  - takePlayerTurn()
  - takeEnemyTurn()
  - basicAttack()

class Database
  + listHeroes()
  + loadCharacter()
  + saveCharacter()
  + printStatistics()
  - createSchema()
  - saveMonsters()
  - saveStats()

class MonsterCatalog
  + monsters()
  + starterHorse()

class Input
  + readIntInRange()
  + readNonEmptyLine()
  + readYesNo()
  + waitForEnter()
```

Relationer til klasse-diagram:

```text
Game --> Input
Game --> MonsterCatalog
Game --> Database
Game --> Battle
Game --> Character
Character *-- Monster
Character *-- GameStats
Monster *-- Item
Monster *-- StatusEffect
Cave *-- Monster
Battle --> Monster
Database --> Character
Database --> GameStats
MonsterCatalog *-- Monster
```
