setwd("C:/SistemasEmTempoReal/T3")
# ANALISE TEMPO DE EXECUCAO

library(tidyverse)
library(dplyr)
library(ggplot2)

dados <- read.csv("resultados_texto2.txt", header = FALSE, sep = ":")

# View(dados)

names(dados)

names(dados) <- c("id","label", "TempoExecucao")

names(dados)

dados$TempoExecucao <- gsub("us", "", dados$TempoExecucao)

# Opcional: Remover espaços extras ao redor dos valores
dados$TempoExecucao <- trimws(dados$TempoExecucao)

View(dados)

dados <- dados %>% 
    filter(TempoExecucao != "") %>%
    filter(!is.na(TempoExecucao)) %>% 
    filter(!is.null(TempoExecucao))

# view(dados)

dados_filtrados <- dados[grepl("^Tempo de execução do", dados$label), ]


dados_filtrados$label <- gsub("Tempo de execução do ", "", dados_filtrados$label)

unique(dados_filtrados$label)

# View(dados)

dados_filtrados$TempoExecucao <- as.numeric(dados_filtrados$TempoExecucao)

ggplot(dados_filtrados, aes(x = TempoExecucao, fill = label)) +
  geom_histogram(color = "black", bins = 30, alpha = 0.8) +
  scale_fill_manual(
    values = c("ABS" = "blue", "Airbag" = "purple")  # Define as cores
  ) +
  facet_wrap(~label, scales = "free") +
  labs(
    title = "Histogramas de Tempo de Execução por Label",
    x = "Tempo de Execução (ms)",
    y = "Frequência"
  ) +
  theme_minimal() +
  theme(
    strip.text = element_text(size = 12, face = "bold"), 
    panel.spacing = unit(1, "cm")
  )

nrow(dados)

dados$TempoExecucao <- as.numeric(dados$TempoExecucao)

dados$id <- as.numeric(dados$id)

exectime_abs <- dados %>% 
  filter(label == "Tempo de execução do ABS")
# summary(exectime_abs)

exectime_airbag <- dados %>% 
  filter(label == "Tempo de execução do Airbag")
# summary(exectime_airbag)

boxplot(
  exectime_abs$TempoExecucao, 
  exectime_airbag$TempoExecucao,
  names = c("ABS", "Airbag"),
  col = c("blue", "purple"),
  main = "Boxplot: Tempo de Execução ABS e Airbag",
  ylab = "Tempo de Execução (us)",
  xlab = "Sistemas"
)

meanexectime_abs <- dados %>% 
  filter(label == "Tempo Médio de Execução do ABS")
# summary(meanexectime_abs)

meanexectime_airbag <- dados %>% 
  filter(label == "Tempo Médio de Execução do Airbag")
# summary(meanexectime_airbag)

meanexectime <- dados %>% 
  filter(label %in% c("Tempo Médio de Execução do ABS", "Tempo Médio de Execução do Airbag"))

meanexectime$label <- gsub("Tempo Médio de Execução do ", "", meanexectime$label)


ggplot(meanexectime, aes(x = id, y = TempoExecucao, color = label)) +
  geom_point(size = 3) +  # Adicionar os pontos
  scale_color_manual(
    values = c("ABS" = "blue", "Airbag" = "purple")
  ) +
  labs(
    title = "Scatter Plot do Tempo Médio de Execução",
    x = "Id da linha",
    y = "Tempo de Execução (us)"
  )


skip_abs <- dados %>% 
  filter(label == "Fator Skip do ABS")

skip_airbag <- dados %>% 
  filter(label == "Fator Skip do Airbag")

boxplot(
  skip_abs$TempoExecucao, 
  skip_airbag$TempoExecucao,
  names = c("ABS", "Airbag"),
  col = c("blue", "purple"),
  main = "Boxplot: Fator Skip ABS e Airbag",
  ylab = "Fator Skip",
  xlab = "Sistemas"
)

wcrt <- dados %>% 
  filter(label %in% c("WCRT do ABS" , "WCRT do Airbag"))

wcrt$label <- gsub("WCRT do ", "", wcrt$label)


ggplot(wcrt, aes(x = id, y = TempoExecucao, color = label)) +
  geom_point(size = 3) + 
  scale_color_manual(  
    values = c("ABS" = "blue", "Airbag" = "purple")
  ) +
  labs(
    title = "Scatter Plot do WCRT",
    x = "Id da linha",
    y = "Tempo de Execução (us)"
  )


wcet <- dados %>% 
  filter(label %in% c("WCET do ABS" , "WCET do Airbag"))

wcrt$label <- gsub("WCET do ", "", wcrt$label)


ggplot(wcrt, aes(x = id, y = TempoExecucao, color = label)) +
  geom_point(size = 3) + 
  scale_color_manual(
    values = c("ABS" = "blue", "Airbag" = "purple")
  ) +
  labs(
    title = "Scatter Plot do WCET",
    x = "Id da linha",
    y = "Tempo de Execução (us)"
  )
