# Problema do histograma em MPI

Compilando usando `make`:

```shell
$ make
mpicc -Wall split-bins.c -o split-bins
```

Ao executar o programa, deve-se inserir os valores de:

* Valores reais mínimos e máximos `<a>` e `<b>` do intervalo
* Valor inteiro da quantidade de números `<data_count>`
* Valor inteiro da quantidade de *bins* `<bin_count>`

Por exemplo, rodando usando `make`:

```shell
$ make run
mpirun ./split-bins
Type in the values for: <a> <b> <data_count> <bin_count>: 0 5 20 5
sequence range: [0.00, 5.00]
sequence length: 20
number of bins: 5
interval: 1.00
[0.00, 1.00) |#################### 3
[1.00, 2.00) |########################## 4
[2.00, 3.00) |############# 2
[3.00, 4.00) |################################# 5
[4.00, 5.00) |######################################## 6
```

