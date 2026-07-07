# SAFEZONE - Monitoramento Inteligente de Segurança Industrial

O **SAFEZONE** é um sistema de segurança baseado em IoT projetado para prevenir acidentes em ambientes industriais. Ele monitora a proximidade de operadores em relação a máquinas perigosas e interrompe o funcionamento do maquinário automaticamente em caso de invasão da área crítica.

---

## Descrição do Problema

Em ambientes fabris, a interação humana com máquinas pesadas oferece riscos constantes. Sistemas de segurança tradicionais muitas vezes são passivos de erros, como esquecer de desligar o maquinário ao fazer uma manutenção. O SAFEZONE resolve isso através de:
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
| Motor                   | 1          | Simula a máquina                    |
| Botões                  | 2          | Parada e reinicialização do sistema |

### 1. Configuração do Hardware
Conecte os componentes seguindo os pinos definidos no código:
*   **PIR:** Pino 15
*   **Ultrassônico:** Trig (18), Echo (19)
*   **LED RGB:** R (5), G (4), B (21)
*   **Buzzer:** Pino 27
*   **Relé:** Pino 2
*   **Botões:** Stop (23), Reset (22)

<img width="805" height="524" alt="image" src="https://github.com/user-attachments/assets/09f1500b-6afe-496f-ba0b-c102ce0507e3" />

## Dashboard de Monitoramento

O SAFEZONE possui um dashboard web desenvolvido para permitir o monitoramento remoto do sistema em tempo real por meio do protocolo MQTT. A interface recebe os dados enviados pelo ESP32 e apresenta informações sobre o estado do ambiente industrial, permitindo que supervisores acompanhem rapidamente possíveis situações de risco.

### Funcionalidades

- Monitoramento em tempo real da distância medida pelo sensor ultrassônico;
- Exibição do status do sensor PIR (presença detectada ou não);
- Indicação do nível de risco do sistema;
- Visualização do estado da máquina (Operando ou Bloqueada);
- Registro das mensagens recebidas via MQTT;
- Botão para redefinição (Reset) do sistema.

### Interface



---

## Demonstração do Projeto

O vídeo abaixo apresenta o funcionamento completo do SAFEZONE, demonstrando a integração entre o ESP32, sensores, relé, sistema de bloqueio da máquina e dashboard web utilizando MQTT.

### Vídeo de Demonstração
