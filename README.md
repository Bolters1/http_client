# HTTP Movie Library Client

## Descriere generala

Acest proiect implementeaza un client HTTP scris in C care comunica cu un REST API pentru gestionarea unei biblioteci de filme.

Clientul permite autentificarea ca administrator sau utilizator normal si ofera functionalitati pentru administrarea utilizatorilor, filmelor si colectiilor de filme prin requesturi HTTP (GET, POST, PUT, DELETE).

---

## Structura proiectului

- **client.c** – logica principala si bucla de comenzi
- **requests.c / requests.h** – construirea requesturilor HTTP
- **helpers.c / helpers.h** – functii auxiliare pentru conexiune si comunicare
- **buffer.c / buffer.h** – buffer pentru primirea raspunsurilor
- **parson.c / parson.h** – biblioteca externa pentru parsarea JSON

---

## Flux de functionare

Clientul ruleaza intr-o bucla in care asteapta comenzi de la stdin. Pentru fiecare comanda:

1. Se deschide o conexiune TCP catre server
2. Se construieste requestul HTTP
3. Requestul este trimis catre server
4. Se primeste si se parseaza raspunsul JSON
5. Se afiseaza rezultatul (SUCCESS sau ERROR)
6. Conexiunea este inchisa

---

## Functionalitati principale

### Administrare utilizatori (admin)

- **login_admin** – autentificare administrator
- **add_user** – adaugare utilizator normal
- **get_users** – afisarea listei de utilizatori
- **delete_user** – stergerea unui utilizator
- **logout_admin** – delogare administrator

### Autentificare utilizator

- **login** – autentificare utilizator normal
- **get_access** – obtinerea tokenului JWT pentru accesul la biblioteca

### Operatii pe filme

- **get_movies** – afisarea tuturor filmelor
- **get_movie** – detalii despre un film
- **add_movie** – adaugarea unui film
- **delete_movie** – stergerea unui film
- **update_movie** – actualizarea informatiilor unui film

### Operatii pe colectii

- **get_collections** – afisarea colectiilor
- **get_collection** – detalii despre o colectie
- **add_collection** – crearea unei colectii noi
- **delete_collection** – stergerea unei colectii
- **add_movie_to_collection** – adaugarea unui film intr-o colectie
- **delete_movie_from_collection** – stergerea unui film din colectie

---

## Detalii de implementare

- Fiecare comanda deschide o conexiune TCP noua catre server
- Cookie-urile si tokenul JWT sunt salvate si trimise ca headere HTTP
- Parsarea JSON este realizata folosind biblioteca **Parson**
- Requesturile HTTP sunt construite manual in `requests.c`