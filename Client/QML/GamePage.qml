import QtQuick 2.9
import QtQuick.Controls.Universal 2.3
import chess.game 1.0
import chess.client 1.0

GamePageForm {
    property int gameType;
    property string str;
    property int time: startWindow.minBox.value*60 + startWindow.secBox.value
    
    onlineButton.onClicked: {
        gameType = Game.ONLINE
        onlineWindow.visible = true
    }
    offlineButton.onClicked: {
        gameType = Game.OFFLINE
        startWindow.visible = true
    }
    
    onlineWindow.closeButton.onClicked: {
        onlineWindow.visible = false
    }
    onlineWindow.acceptButton.onClicked: {
        Client.connectToGame(onlineWindow.gameList.currentIndex);
    }
    onlineWindow.createButton.onClicked: {
        if (onlineWindow.createButton.down) {
            onlineWindow.visible = true
            onlineWindow.state = "CONNECTED"
            startWindow.visible = false
            Client.deleteGame();
        } else {
            onlineWindow.enabled = false
            startWindow.visible = true
            startWindow.humanButton.checked = true
        }
    }
    onlineWindow.serverButton.onClicked: {
        if (onlineWindow.serverButton.down) {
            onlineWindow.state = "DISCONNECTED"
            Client.disconnectFromServer();
        } else {
            onlineWindow.state = "CONNECTING"
            Client.connectToServer(onlineWindow.serverField.text);
        }
    }
    onlineWindow.refreshButton.onClicked: {
        Client.updateGames();
    }
    
    
    startWindow.humanButton.onClicked: {
        startWindow.whiteButton.checked = true
    }
    startWindow.cancelButton.onClicked: {
        if (onlineWindow.visible) 
            onlineWindow.enabled = true
        startWindow.visible = false
    }
    startWindow.startButton.onClicked: {
        startWindow.visible = false
        chatWindow.finishButton.visible = false
        if (onlineWindow.visible) {
            onlineWindow.enabled = true
            onlineWindow.state = "WAITING"
            
            Client.createGame(startWindow.blackButton.checked, time);
        } else {
            if (startWindow.aiButton.checked)
                gameType = Game.AI;
            boardWindow.visible = true
            if (time) {
                whiteClock.visible = true
                blackClock.visible = true
            }
            Game.startGame( gameType, 
                            startWindow.blackButton.checked, 
                            time);
        }
    }
    
    chatWindow.finishButton.onClicked: {
        if (gameType === Game.ONLINE) {
            if (chatWindow.disconnectButton.enabled)
                Client.disconnectFromGame();
            onlineWindow.visible = true
            chatWindow.dialogueText.clear();
            Client.updateGames();
        }
        boardWindow.visible = false
        whiteClock.visible = false
        blackClock.visible = false
    }
    chatWindow.claimDrawButton.onClicked: {
        Game.claimDraw();
        Client.sendAction("\x08");
    }
    chatWindow.offerDrawButton.onClicked: {
       Client.sendAction("\x0f");
       chatWindow.offerDrawButton.enabled = false
       chatWindow.drawTimer.start();
    }
    chatWindow.resignButton.onClicked: {
        if (gameType === Game.ONLINE) {
            Client.sendAction("\x04");
            chatWindow.dialogueText.append("<i><font color=\"#ee1111\">YOU RESIGNED - OPPONENT WON</font></i>");
        }
        chatWindow.finishButton.visible = true
        Game.statusChanged(Game.RESIGN, Game.getSide());
        Game.endGame();
    }
    chatWindow.drawTimer.onTriggered: {
        chatWindow.offerDrawButton.enabled = true
    }
    chatWindow.disconnectButton.onClicked: {
        Client.disconnectFromGame();
    }
    chatWindow.messageField.onEditingFinished: {
        if (chatWindow.messageField.text.length)
            chatWindow.sendButton.clicked();
    }
    chatWindow.sendButton.onClicked: {
        Client.sendMessage(chatWindow.messageField.text);
        chatWindow.dialogueText.append("<font color=\"#888888\">You: "+chatWindow.messageField.text+"</font>");
        chatWindow.messageField.clear();
    }
    
    drawWindow.drawYesButton.onClicked: {
        drawWindow.visible = false
        boardWindow.enabled = true
        chatWindow.enabled = true
        Client.sendAction("\x08");
        Game.claimDraw();
    }
    drawWindow.drawNoButton.onClicked: {
        drawWindow.visible = false
        boardWindow.enabled = true
        chatWindow.enabled = true
    }
    
    queenButton.onClicked: {
        promotionWindow.visible = false
        Game.enabled = true;
        Game.promote(45056);
    }
    rookButton.onClicked: {
        promotionWindow.visible = false
        Game.enabled = true;
        Game.promote(40960);
    }
    bishopButton.onClicked: {
        promotionWindow.visible = false
        Game.enabled = true;
        Game.promote(36864);
    }
    knightButton.onClicked: {
        promotionWindow.visible = false
        Game.enabled = true;
        Game.promote(32768);
    }
    
    Connections {
        target: Game
        onStatusChanged: {
            str = turn ? "Black" : "White";
            switch (status) {
                case Game.REGULAR:   gameStatus.text = qsTr(str+"'s turn"); break;
                case Game.CHECK:     gameStatus.text = qsTr(str+"'s in check"); break;
                case Game.CHECKMATE: gameStatus.text = qsTr((turn ? "White" : "Black")+" won"); break;
                case Game.STALEMATE: gameStatus.text = qsTr("Stalemate"); break;
                case Game.DRAW:      gameStatus.text = qsTr("Draw"); break;
                case Game.RESIGN:    gameStatus.text = qsTr(str+" resigned"); break;
            }
            if (turn) {
                blackClock.state = "RUNNING"
                whiteClock.state = "STOPPED"
            } else {
                whiteClock.state = "RUNNING"
                blackClock.state = "STOPPED"
            }
        }
        onTimeChanged: {
            if (turn)
                blackClock.clockTime.text = Qt.formatTime(time, "mm:ss:z")
            else
                whiteClock.clockTime.text = Qt.formatTime(time, "mm:ss:z")
        }
        onDrawAvailable: {
            chatWindow.claimDrawButton.enabled = draw;
        }
        onGameEnded: {
            chatWindow.finishButton.visible = true;
        }
        onGetPromotion: {
            promotionWindow.visible = true
        }
    }
    
    Connections {
        target: Client
        onConnectedToServer: {
            onlineWindow.state = "CONNECTED"
            Client.updateGames();
        }
        onDisconnectedFromServer: {
            onlineWindow.state = "DISCONNECTED"
        }
        
        onConnectedToPlayer: {
            onlineWindow.state = "CONNECTED"
            onlineWindow.visible = false
            
            chatWindow.ipField.text = Client.getOpponentIp();
            chatWindow.disconnectButton.enabled = true
            chatWindow.offerDrawButton.enabled = true
            chatWindow.finishButton.enabled = false
            chatWindow.dialogueText.append("<i><font color=\"#ee1111\">GAME STARTED</font></i>");
            
            boardWindow.visible = true
            if (Client.getTime()) {
                whiteClock.visible = true
                blackClock.visible = true
            }
            Game.startGame(gameType, Client.getSide(), Client.getTime());
        }
        onDisconnectedFromPlayer: {     
            chatWindow.ipField.clear();
            chatWindow.disconnectButton.enabled = false
            chatWindow.offerDrawButton.enabled = false
            chatWindow.finishButton.enabled = true
            chatWindow.dialogueText.append("<i><font color=\"#ee1111\">DISCONNECTED</font></i>");
            
            Game.endGame();
        }
        
        onUpdateDialogue: {
            chatWindow.dialogueText.append("Opponent: "+message);
        }
        onImportantMsg: {
            chatWindow.dialogueText.append("<i><font color=\"#ee1111\">"+message+"</font></i>");
        }
        onDrawOffer: {
            drawWindow.visible = true
            boardWindow.enabled = false
            chatWindow.enabled = false
        }
        
        onServerError: {
            Client.disconnectFromServer();
            onlineWindow.state = "DISCONNECTED"
            onlineWindow.onlineLabel.text = "Error: "+errorString
        }
        onPlayerError: {
            console.log("Player error: "+errorString);
        }
    }
}
