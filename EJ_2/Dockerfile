FROM openjdk:17-slim

WORKDIR /app

COPY ./Servidor.java .
COPY ./Cliente.java .

# Compilar indicando el classpath actual
RUN javac -cp . Servidor.java Cliente.java

# Exponer el puerto
EXPOSE 4000

# Ejecutar con classpath explícito
CMD ["java", "-cp", ".", "Servidor"]
