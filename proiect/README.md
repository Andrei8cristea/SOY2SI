# Encriptor / Decriptor

## Utilizare

./encriptor <inputFile>                     → encriptare
./encriptor <inputFile> <permutationFile>   → decriptare

## Algoritm
Programul foloseste **Fisher–Yates shuffle** pentru a genera o permutare aleatorie a indexurilor unui cuvant.  
Permutarea este aplicata pentru a reordona caracterele.

---

## Encriptare
- Deschide fisierul de input.  
- Obtine dimensiunea si il mapeaza in memorie.  
- Deschide fisierele `encrypted.txt` si `permutations.txt`.  
- Parcurge tot continutul si identifica fiecare cuvant.  
- Pentru fiecare cuvant:  
  - Creeaza un proces copil.  

### Copilul
- copiaza cuvantul intr-un buffer  
- genereaza permutarea (seed random)  
- aplica permutarea  
- trimite parintelui lungimea, cuvantul encriptat si permutarea  

### Parintele
- citeste datele din pipe  
- scrie cuvantul encriptat in `encrypted.txt`  
- scrie permutarea in `permutations.txt`  
- separa cuvintele si permutarile prin spatiu / newline  
- asteapta copilul  

---

## Decriptare
- Deschide `encrypted.txt` si `permutations.txt`.  
- Creeaza `decrypted.txt`.  
- Pentru fiecare cuvant encriptat:  
  - sare peste whitespace  
  - citeste caracter cu caracter pana la urmatorul delimitator  
  - daca ajunge la EOF, iese  
- Citeste linia de permutare corespunzatoare.  
- Construieste vectorul de permutare (folosind `atoi`).  
- Construieste inversa permutarii.  
- Reconstruieste cuvantul original aplicand inversa.  
- Scrie cuvantul decriptat in `decrypted.txt`, separat prin spatiu.  
