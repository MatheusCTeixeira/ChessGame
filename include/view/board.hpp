#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>

#include "../model/type.hpp"
#include "../model/color.hpp"
#include "../model/game.hpp"

#include "../controller/boardController.hpp"

#include <SFML/Graphics.hpp>


namespace ch {

class CBoardView: public sf::Drawable {
    using TypeColor = std::tuple<ch::EType, ch::EColor>;

    struct key_hash: public std::unary_function<TypeColor, std::size_t> {
        std::size_t operator() (const TypeColor& tp) const;
    };

public:
    CBoardView(
        sf::FloatRect screenDimensions,
        std::string pieceTexFile,
        std::string boardTexFile
    );

    virtual ~CBoardView();

    // Inicializa o jogo.
    void init();

    // Trata eventos como cliques do mouse.
    void onClick(const sf::Event& mousePosition);

    // Determina a região em que o jogo deve ser exibido. Útil para reservar es-
    // paça para as demais funcionalidades como placar ou lista de movimentos
    // em outras regiões da tela.
    void setBoardDimensions(sf::FloatRect screenDimensions);
    sf::FloatRect getBoardDimensions() const;

    // Desenha o jogo.
    void draw(sf::RenderTarget& target, sf::RenderStates state) const override;

protected:
    // Transforma as coordenadas do tabuleiro para as coordenadas do View.
    sf::Vector2f coordToBoardLocation(int i, int j) const;

    // Transforma as coordenadas do View para as coordenadas do tabuleiro.
    ch::CPosition boardLocationToCoord(int x, int y) const;

    // Carrega os Sprites das peças.
    void loadPieces();

    // Carrega os Sprites das regiões do tabuleiro.
    void loadBoardPositionColor();

    // Escala cada Sprite (das peãs e das regiẽos) para adequá-las às dimenssões
    // da tela.
    void scaleSprites();

    // Obtém as dimenções de uma peça após ser escalada. Útil para destarcar a
    // região/peça selecionada no tabuleiro.
    sf::Vector2f getPieceDimentions() const;

    void highlightSelectedPiece(
        ch::CPosition pos, sf::RenderTarget& target) const;

    void highlightPossibleMoves(
        ch::CPosition pos, bool isOccupied, sf::RenderTarget& target) const;

    // Destaca a peça selecionada.
    void drawSelectedPiece(sf::RenderTarget& target) const;

    // Desenha o tabuleiro.
    void drawBoard(sf::RenderTarget& target) const;

    // Desenha as peças.
    void drawPieces(sf::RenderTarget& target) const;

    // Mostra os possiveis movimentos de uma peça, quando alguma for seleciona-
    // da.
    void showPossibleMoves(sf::RenderTarget& target) const;

    // Verifica que é a ação é de movimento, isto é, uma peça está selecionada
    // e uma posição desejado foi pressionada.
    void handleMove();

    // Realiza o movimento propriamente.
    void move(CPosition from, CPosition to);

    // Armazena a textura que contém todas as peças.
    sf::Texture _piecesTex;

    // Armazena a textura das cores do tabuleiro.
    sf::Texture _boardTex;

    // Contém cada peça individualmente. A mesma peça pode ser utilizada diver-
    // sas vezes, por exemplo, quando ocorre a promoção o peão pode se tornar
    // qualquer peça.
    std::unordered_map<
        TypeColor,
        sf::Sprite,
        key_hash> _pieces;

    // Contém os Sprites das regiões do tabuleiro.
    std::unordered_map<
        EColor,
        sf::Sprite> _boardPositionColor;

    // TODO Mover para o Controller.
    CBoardController _gameController;

private:
    sf::FloatRect _screenDimensions;

    // TODO Refatorar. Acho que não será necessário utilizar um sf::View.
    sf::View _gameView;
    sf::Vector2i _selectedPoint;

    std::shared_ptr<CPiece> _selectedPiece;

    std::string _pieceTexFile;
    std::string _boardTexFile;
};

}