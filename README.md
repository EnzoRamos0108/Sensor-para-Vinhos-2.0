# 🍷 Vinheria Agnello - Monitoramento Inteligente de Ambiente

Bem-vindo ao repositório do projeto de **Monitoramento de Luminosidade, Temperatura e Umidade** da **Vinheria Agnello**. Este sistema foi desenvolvido com **Arduino** para garantir as melhores condições de armazenamento dos vinhos, protegendo-os contra variações que possam comprometer sua qualidade.

---

## 📌 Objetivo

- Monitorar em tempo real:
  - **Luminosidade**
  - **Temperatura**
  - **Umidade**
- Alertar alterações no ambiente utilizando:
  - **LEDs (verde, amarelo e vermelho)** para indicar o status (estável, alerta ou crítico).
  - **Buzzer** com sons diferentes para situações de alerta e crítico.
- **Registrar na EEPROM** todos os dados quando o ambiente estiver fora dos parâmetros ideais.
- **Exibir no display LCD I2C:**
  - Estado atual do ambiente (Estável, Alerta ou Crítico) com emojis.
  - Permitir alternar entre o estado e os índices de luminosidade, temperatura e umidade.
- **Animação de um cavalo correndo** no início do sistema como assinatura do projeto. 🐎

---

## ⚙️ Estrutura do Projeto

O sistema funciona da seguinte forma:

- Ao ligar, é exibida uma **animação de cavalo correndo** no display LCD 16x2.
- O usuário escolhe o tipo de vinho armazenado (**Tinto, Branco ou Ambos**) utilizando dois botões:
  - Um para **navegar** entre as opções.
  - Outro para **confirmar** a seleção.
- Assim, vai ser feita a calibração das temperaturas mínima e máxima de acordo com o tipo de vinho armazenado.
- Após a seleção, é feita a **calibração dos níveis de luminosidade mínima e máxima** por meio de botão.
- O sistema lê constantemente:
  - **Luminosidade** (via LDR)
  - **Temperatura e umidade** (via sensor DHT11)
- Os dados são processados e comparados com os parâmetros ideais definidos para o tipo de vinho escolhido.
- O display exibe o **estado atual** do ambiente:
  - 😃 **Ambiente Estável**
  - 😐 **Ambiente em Alerta**
  - 😟 **Ambiente Crítico**
- Caso esteja em alerta ou crítico:
  - O buzzer emite sons periódicos (3 segundos ligado, 1 segundo desligado).
  - O LED correspondente (amarelo ou vermelho) acende.
  - Os dados são **registrados na EEPROM** com data, hora, temperatura, umidade e luminosidade.
- O monitor serial permite visualizar todos os registros de anomalias armazenados na EEPROM.

---

## 🔥 Funcionalidades Diferenciais

- Escolha do tipo de vinho com botões e mensagens no display.
- Adaptação dos limites de temperatura de acordo com o tipo de vinho escolhido.
- Calibração manual dos níveis de luminosidade feita pelo usuário através de botões.
- Sistema inteligente de controle de memória que administra os registros na EEPROM para que ela não fique cheia.
- Alternância no LCD entre o status do ambiente e os índices detalhados (feita por botões).

---

## 📦 Componentes Utilizados

### 🛠️ Hardware:

- 1x Arduino Uno
- 1x Sensor de Luminosidade (LDR)
- 1x Sensor de Temperatura e Umidade DHT11 (utilizar o DHT22 na simulacão do Wokwi)
- 1x Display LCD 16x2 com I2C
- 1x Módulo RTC DS1307 (Relógio de tempo real)
- 1x Buzzer
- 3x LEDs (verde, amarelo e vermelho)
- 3x Resistores (para os LEDs)
- 2x Botões (Navegar e Selecionar)
- Jumpers e Protoboard

### 📚 Bibliotecas:

- `Wire.h` (I2C)
- `LiquidCrystal_I2C.h` (Display LCD I2C)
- `RTClib.h` (RTC DS1307)
- `EEPROM.h` (Memória EEPROM)
- `DHT.h` (Sensor DHT)

---

## 🚀 Como Executar

1. Clone ou baixe este repositório.
2. Abra o arquivo `sensorLumUmiTemp.ino` na **IDE do Arduino**.
3. Monte o circuito fisicamente ou no simulador (Wokwi) utilizando os componentes listados ou copie o `diagram.json` no link da nossa simulação.
4. Faça o upload do código para a placa Arduino Uno.
5. Baixe as bibliotecas informadas acima.
6. Acompanhe no LCD a animação, realize a seleção do tipo de vinho e veja o monitoramento do ambiente em tempo real.
7. Utilize o **Monitor Serial** para visualizar os registros salvos na EEPROM.

---

## 🎬 Demonstração

### 📽️ Vídeo da implementação:

🔗 *[https://youtu.be/ykm4L_7e5WI?si=XMpc8vxqnntVoopi]*

### 📸 Simulação no Wokwi:

🔗 *[https://youtu.be/ykm4L_7e5WI?si=XMpc8vxqnntVoopi]*

---

## 🤝 Contribuições

Este projeto foi desenvolvido como parte da disciplina de **Edge Computing and Computer Systems** com foco na integração de hardware e software, aplicando conceitos de sensores, atuadores, processamento de dados e registro de eventos.

### 🌟 Destaques:

- Sistema inteligente com armazenamento do histórico de anomalias.
- Display com animação e interface intuitiva.
- Alertas visuais e sonoros robustos.
- Código comentado e organizado para fácil entendimento.

---

## 📄 Licença

Este projeto é de uso **educacional e acadêmico**. Sinta-se livre para estudar, adaptar e se inspirar! ✨

---

> Desenvolvido com dedicação, criatividade e muitas horas de café por Enzo Ramos, Felipe Cerazi, Gustavo Peaguda e Lorenzo Coque 💻🍷
