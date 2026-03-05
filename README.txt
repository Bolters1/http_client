Descriere generala:
    Implementarea unui client HTTP in C care comunica cu un REST API pentru
    gestionarea unei baze de date de filme. Clientul permite autentificarea
    ca admin sau utilizator normal, gestionarea filmelor si a colectiilor
    de filme, prin intermediul requesturilor HTTP (GET, POST, PUT, DELETE).

Structura fisierelor:
    - client.c    : logica principala a clientului (bucla de comenzi)
    - requests.c  : constructia requesturilor HTTP (GET, POST, PUT, DELETE)
    - requests.h  : headerele functiilor din requests.c
    - helpers.c   : functii auxiliare (conexiune, trimitere/primire mesaje)
    - helpers.h   : headerele functiilor auxiliare
    - buffer.c/h  : implementare buffer pentru primirea raspunsurilor
    - parson.c/h  : biblioteca externa pentru parsarea si crearea JSON-urilor

Flux de functionare:
    Clientul ruleaza intr-o bucla infinita asteptand comenzi de la stdin.
    La fiecare comanda, se deschide o noua conexiune TCP catre server
    (63.32.125.183:8081), se construieste si trimite cererea HTTP, se
    parseaza raspunsul JSON si se afiseaza rezultatul (SUCCESS/ERROR).

Comenzi implementate:
    
    1. login_admin
       - Autentificare ca administrator
       - Cere: username, password
       - Trimite POST la /api/v1/tema/admin/login cu JSON-ul credentialelor
       - Salveaza cookie-ul de sesiune primit de la server

    2. add_user
       - Adauga un utilizator normal (necesita rol de admin)
       - Cere: username, password
       - Trimite POST la /api/v1/tema/admin/users cu cookie-ul de admin

    3. get_users
       - Afiseaza toti utilizatorii normali (necesita rol de admin)
       - Trimite GET la /api/v1/tema/admin/users
       - Parseaza raspunsul JSON si afiseaza lista sub forma #id user:pass

    4. delete_user
       - Sterge un utilizator normal (necesita rol de admin)
       - Cere: username
       - Trimite DELETE la /api/v1/tema/admin/users/:username

    5. logout_admin
       - Delogheaza adminul curent
       - Trimite GET la /api/v1/tema/admin/logout
       - Reseteaza cookie-ul de admin

    6. login
       - Autentificare ca utilizator normal
       - Cere: admin_username, username, password
       - Trimite POST la /api/v1/tema/user/login cu toate datele
       - Salveaza cookie-ul de sesiune al utilizatorului

    7. get_access
       - Obtine token JWT pentru accesul la biblioteca de filme
       - Necesita sa fie logat un utilizator normal
       - Trimite GET la /api/v1/tema/library/access
       - Extrage si salveaza tokenul JWT din raspuns ca header Authorization

    8. get_movies
       - Afiseaza toate filmele din biblioteca (necesita token JWT)
       - Trimite GET la /api/v1/tema/library/movies
       - Parseaza array-ul de filme si afiseaza #id titlu

    9. get_movie
       - Afiseaza detaliile unui film specific (necesita token JWT)
       - Cere: id
       - Trimite GET la /api/v1/tema/library/movies/:id
       - Afiseaza: title, year, description, rating

    10. add_movie
        - Adauga un film in biblioteca (necesita token JWT)
        - Cere: title, year, description, rating
        - Trimite POST la /api/v1/tema/library/movies cu JSON-ul filmului

    11. delete_movie
        - Sterge un film din biblioteca (necesita token JWT)
        - Cere: id
        - Trimite DELETE la /api/v1/tema/library/movies/:id

    12. update_movie
        - Actualizeaza datele unui film existent (necesita token JWT)
        - Cere: id, title, year, description, rating
        - Trimite PUT la /api/v1/tema/library/movies/:id cu JSON-ul nou

    13. get_collections
        - Afiseaza toate colectiile de filme (necesita token JWT)
        - Trimite GET la /api/v1/tema/library/collections
        - Serverul poate returna un array JSON sau un obiect cu eroare
        - Trateaza ambele cazuri: parseaza ca array daca e array direct,
          sau verifica campul "error" daca e obiect

    14. get_collection
        - Afiseaza detaliile unei colectii specifice (necesita token JWT)
        - Cere: id
        - Trimite GET la /api/v1/tema/library/collections/:id
        - Afiseaza: title, owner si lista filmelor din colectie

    15. add_collection
        - Creeaza o noua colectie de filme (necesita token JWT)
        - Cere: title, num_movies si movie_id pentru fiecare film
        - Mai intai trimite POST la /api/v1/tema/library/collections cu titlul
        - Primeste ID-ul colectiei create
        - Apoi trimite cate un POST la /collections/:id/movies pentru fiecare
          film specificat, cu payload-ul {"id": movie_id}

    16. delete_collection
        - Sterge o colectie de filme (necesita token JWT, trebuie sa fii owner)
        - Cere: id
        - Trimite DELETE la /api/v1/tema/library/collections/:id

    17. add_movie_to_collection
        - Adauga un film intr-o colectie existenta (necesita token JWT)
        - Cere: collection_id, movie_id
        - Trimite POST la /api/v1/tema/library/collections/:collection_id/movies
          cu payload-ul {"id": movie_id}

    18. delete_movie_from_collection
        - Sterge un film dintr-o colectie (necesita token JWT, trebuie owner)
        - Cere: collection_id, movie_id
        - Trimite DELETE la /collections/:collection_id/movies/:movie_id

    19. logout
        - Delogheaza utilizatorul normal curent
        - Trimite GET la /api/v1/tema/user/logout
        - Reseteaza cookie-ul de utilizator, tokenul JWT si statusul de login

    20. exit
        - Inchide programul fara afisarea unui mesaj

Detalii de implementare:
    - Fiecare comanda deschide o conexiune noua TCP si o inchide dupa primirea
      raspunsului, pentru a evita problemele cu conexiuni persistente
    - Cookie-urile si token-ul JWT sunt stocate ca headere complete
      (ex: "Cookie: connect.sid=..." sau "Authorization: Bearer ...")
      si sunt trimise direct ca headere in requesturi
    - Parsarea JSON se face cu biblioteca Parson (parson.c/parson.h)
    - Toate raspunsurile cuprind cuvintele SUCCESS sau ERROR conform cerintei
    - Constructia requesturilor HTTP este facuta manual, fara biblioteci externe
      de HTTP, folosind functiile din requests.c