# Autores:
- Gomez Luis (lgomez@patagones.cl)
- Iriarte Nicolás (NicolasIriarte95@gmail.com)
- Edda Andrade Rosales (edda.geraldine.andrade.rosales@gmail.com) desde TP1 B y TP2 A

# Enunciado:
Se solicita implementar una solución para el siguiente sistema.
- [X] El sistema cuenta con dos LEDs, LED verde y LED rojo, y un botón.
- [ ] El sistema inicia con los LEDs apagados.
- [ ] El sistema tendrá que detectar entre distintos modos de presionar el botón.
- [ ] Se considera botón corto cuando el botón se presiona durante 100 y 2000 ms.
- [ ] Se considera botón largo cuando el botón se presiona durante 2000 y 8000 ms.
- [ ] Se considera botón trabado cuando el botón se mantiene presionado más de 8000
ms.
- [ ] Si se detecta un botón corto se debe invertir el estado (toogle) del LED verde.
- [ ] Si se detecta un botón largo se debe invertir el estado (toogle) del LED rojo.
- [ ] Si se detecta un botón trabado se deben encender los LEDs verde y rojo.
- [ ] Si desaparece la condición de botón trabado se deben apagar los LEDs verde y
rojo.

# Requerimientos
- Cumplir con la descripción de comportamiento del sistema.
- Implementar la solución con el patrón de diseño de sistemas reactivos.
- La solución propuesta con el patrón de sistemas reactivos debe solucionar los
problemas de concurrencia.
- Se debe entregar un proyecto compatible con el IDE STM32Cube

# Otros:
Podrá encontrar el enunciado comple [aqui](assets/D05_TP1_ParteA_v0p2.pdf)

Link al repositorio: [https://github.com/NicolasIriarte/RTOS-II-PW1-A](https://github.com/NicolasIriarte/RTOS-II-PW1-A)
