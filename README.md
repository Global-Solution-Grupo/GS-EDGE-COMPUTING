# 💧 Sistema de Monitoramento de Nível de Água com Arduino

## ⚠️ Problema

As **enchentes** são eventos cada vez mais frequentes que causam grandes prejuízos materiais e riscos à vida. Muitas comunidades sofrem com a falta de um sistema de alerta rápido e acessível para detectar a iminência de uma enchente.

## ✅ Solução

Este projeto propõe um **sistema de monitoramento de nível de água** utilizando **Arduino**, sensores de presença (para detecção de água em diferentes alturas), **RTC (Relógio de Tempo Real)**, **EEPROM** (para registro histórico), e **LCD** (para exibição das informações).

O sistema detecta o nível da água em **3 estágios**:
- 🟢 **Sem risco**: Nenhum sensor ativo.
- 🟡 **Risco moderado**: Apenas o sensor mais baixo detecta água.
- 🟠 **Risco alto**: Dois sensores detectam água.
- 🔴 **Risco crítico (enchente iminente)**: Todos os sensores detectam presença de água.

Além disso, os dados de alerta são **registrados na EEPROM** com carimbo de data/hora e podem ser **exportados via Serial (CSV)**.

---

## ⚙️ Componentes Utilizados

- Arduino UNO
- 3 sensores de presença (simulando níveis de água)
- RTC DS1307
- LCD 16x2 com I2C
- EEPROM (interna do Arduino)
- LEDs (Verde, Amarelo, Laranja, Vermelho)
- Buzzer

---

## 🖥️ Exibição no LCD

O display exibe:
- Hora atual
- Mensagens de alerta em tempo real

---

## 🔔 Sistema de Alerta

Cada nível aciona:
- LED correspondente
- Mensagem no Serial Monitor
- Mensagem no LCD
- Som de alerta com o buzzer (por 10 segundos)

---

## 💾 Registro na EEPROM

Cada alerta registrado armazena:
- Timestamp (Unix)
- Nível de alerta (0 = sem risco, até 3 = enchente iminente)

---

## 📤 Exportação dos Dados

Para exportar os dados armazenados na EEPROM, basta digitar `export` no Serial Monitor.

Saída em CSV:

Timestamp,Nivel
1620000000,0
1620000600,1
...


---
🔄 Fluxograma do Sistema
A imagem abaixo ilustra o fluxograma do funcionamento do sistema de monitoramento de nível de água. Ele descreve passo a passo a lógica implementada no código Arduino, desde a inicialização dos componentes até a identificação de riscos de enchente e o registro dos eventos na EEPROM.

Cada decisão e ação é baseada na média de 10 leituras dos sensores de presença, que estão posicionados em diferentes alturas. Dependendo do número de sensores ativados, o sistema classifica a situação em quatro níveis de risco:

Sem risco (LED Verde): Nenhum sensor detecta água.

Risco baixo (LED Amarelo): Apenas o sensor mais inferior detecta água.

Risco alto (LED Laranja): Dois sensores detectam água.

Enchente iminente (LED Vermelho): Todos os sensores detectam presença de água.

Após cada verificação, o nível de alerta e o horário são salvos na memória EEPROM para futuras análises ou exportação via porta serial.

Esse fluxograma facilita o entendimento do comportamento do sistema, tornando o projeto mais acessível para manutenção, expansão ou simulação.

![image](https://github.com/user-attachments/assets/4fefd051-fd71-4e9c-b756-a7934d177663)

---

## 🧪 Simulação no Wokwi

### ✅ Como simular no [Wokwi](https://wokwi.com)

1. Acesse o link abaixo:
   [🔗 Simular Projeto no Wokwi](https://wokwi.com/projects/432676521923276801)
2. Pressione os botões dos sensores virtuais para simular a subida do nível da água.
3. Acompanhe as mensagens no Serial Monitor e no LCD virtual.

### 💡 Dica:
Use o botão "Simular" no topo da tela do Wokwi e ative os sensores em sequência para testar os diferentes níveis de alerta.

---

## 🎥 Vídeo Demonstrativo

[▶️ Assistir ao vídeo demonstrativo]()

---

## 📂 Estrutura do Código

- `setup()` → Inicialização dos pinos, RTC e LCD.
- `loop()` → Coleta dos dados, cálculo de médias e emissão de alertas.
- `checkSerialCommands()` → Verifica comandos via serial (`export`) e imprime dados CSV.

---

## 📎 Licença

Este projeto está sob a licença MIT. Sinta-se livre para usar, modificar e compartilhar.

---

## 👨‍💻 Autor

Desenvolvido por **[Ronaldo Aparecido Monteiro Almeida, Henrique Infanti Coratolo e Lucas Rowlands Abat]** – Projeto Arduino para prevenção de enchentes.

