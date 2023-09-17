# Trabalho_Final_CPD

Este arquivo utiliza três arquivos .csv:

players.csv, contendo informções sobre jogadores avaliações de usuários e anotações em
texto-livre (tags). O arquivo players.csv contém informações de 18.944 jogadores, composto
de um id no FIFA, seu nome e uma lista de posições.

O arquivo rating.csv contém 24,188,078 de avaliações (notas entre 1 e 5) de usuários para
jogadores. Esses dados foram geradores simulando avaliações de usuários para cada jogador
disponível no dataset.

O arquivo tag.csv contém 364,950 anotações de texto livre (tags) (ex.: Brazil, FK Specialist,
Speedster, Playmaker, Paris Saint-Germain) para 18,944 jogadores.

O objetivo do trabalho é implementar estruturas de dados e algoritmos que suportam as
seguintes pesquisas sobre os dados:

--Pesquisas sobre os nomes de jogadores:

Esta pesquisa tem por objetivo retornar a lista de jogadores cujo nome começa com um texto
que pode ser o prefixo ou texto completo do nome de um jogador. Todos os jogadores que
satisfizerem o texto de consulta devem ser retornados, junto com a lista de posições dos
jogadores, avaliação média e número de avaliações.

Uma primeira observação é que os dados de jogadores não contêm algumas das informações
a serem retornadas. Por exemplo, deve-se primeiramente guardar em uma tabela hash as
médias de avaliações e total de avaliações para cada jogador. Essas informações devem ser
calculadas e armazenadas em uma etapa de pré-processamento. Para responder esta
pesquisa, deve-se implementar uma árvore trie que busca todos os identificadores de
jogadores que correspondem ao nome ou prefixo dado, e com essa lista de identificadores,
buscar na tabela hash as informações complementares dos jogadores.

--Pesquisas sobre jogadores revisados por usuários:

Esta pesquisa deve retornar a lista com no máximo 20 jogadores revisados pelo usuário e para
cada jogador mostrar a nota dada pelo usuário, a média global e a contagem de avaliações. A
lista deve ser ordenada pela nota dada pelo usuário de maneira decrescente (maiores notas
primeiro).

Para resolver essa questão, criou-se uma tabela hash para todos os usuários, contendo a id do usuário, um array com as avaliações
um array com as avaliações feitas pelo usuário, o número de avaliações feitas pelo usuário e o tamanho de avaliações.

--Pesquisas sobre os melhores jogadores de uma determinada
posição:

Esta pesquisa tem por objetivo retornar a lista de jogadores com melhores notas de uma dada
posição. Para evitar que um jogador seja retornando com uma boa média mas com poucas
avaliações, esta consulta somente deve retornar os melhores jogadores com no mínimo 1000
avaliações. Para gerenciar o número de jogadores a serem retornados, a consulta deve
receber como parâmetro um número N que corresponde ao número máximo de jogadores a
serem retornados

Para resolver essa questão, criou-se uma lista de posições, contendo a lista de jogadores que jogam em determinada posição.

--Pesquisas sobre ‘tags’ de jogadores:

Esta pesquisa tem por objetivo explorar a lista de tags adicionadas por cada usuário em cada
revisão. Para uma lista de tags dada como entrada, a pesquisa deve retornar a lista de
jogadores que estão associados a interseção de um conjunto de tags.

Para resolver essa questão, criou-se uma tabela hash de tags, contendo um array com dados de tags, o tamanho da tabela,
e o número de dados a serem inseridos.
