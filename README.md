# SAFEZONE - Monitoramento Inteligente de Segurança Industrial

O **SAFEZONE** é um sistema de segurança baseado em IoT projetado para prevenir acidentes em ambientes industriais. Ele monitora a proximidade de operadores em relação a máquinas perigosas e interrompe o funcionamento do maquinário automaticamente em caso de invasão da área crítica.

---

## Descrição do Problema

Em ambientes fabris, a interação humana com máquinas pesadas oferece riscos constantes. Sistemas de segurança tradicionais muitas vezes são passivos de erros, como esqquecer de deligar o maquinário ao fazer uma manutenção. O SAFEZONE resolve isso através de:
1.  **Monitoramento Ativo:** Detecção de presença e distância em tempo real.
2.  **Intervenção Automática:** Bloqueio imediato ao detectar invasão na área de risco.
3.  **Transparência de Dados:** Dashboard em tempo real para supervisores acompanharem o status de segurança via MQTT.

---

## Componentes e Instalação

| Componente              | Quantidade | Função                              |
|-------------------------|------------|-------------------------------------|
| ESP32                   | 1          | Microcontrolador principal          |
| Sensor Ultrassônico     | 1          | Medição de distância                |
| Sensor PIR              | 1          | Detecção de movimento               |
| Módulo Relé             | 1          | Acionamento de cargas externas      |
| LED RGB                 | 1          | Indicação visual de estados         |
| Buzzer                  | 1          | Alarme sonoro                       |
| Botões                  | 2          | Parada e reinicialização do sistema |

### 1. Configuração do Hardware
Conecte os componentes seguindo os pinos definidos no código:
*   **PIR:** Pino 15
*   **Ultrassônico:** Trig (18), Echo (19)
*   **LED RGB:** R (5), G (4), B (21)
*   **Buzzer:** Pino 27
*   **Relé:** Pino 2
*   **Botões:** Stop (23), Reset (22)

<img width="761" height="516" alt="image" src="https://github.com/user-attachments/assets/0a1805cd-e5f5-481b-a2c3-d2a4f8f56449" />



