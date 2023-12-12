# MRFStr library functions performance

## mrfstr_set

```python
a = '0' * (1024 * 1024 * 1024) # 1GB
b = a[:-1]
```

* **Python:** 261.20 milliseconds (3.828 GB/s)
* **C:** 114.54 milliseconds (8.731 GB/s)
* **MRFStr:** 43.98 milliseconds (22.738 GB/s)

## mrfstr_concat

```python
a = '0' * (1024 * 1024 * 512) # 0.5GB
b = '0' * (1024 * 1024 * 512) # 0.5GB
c = a + b # 1GB
```

* **Python:** 326.40 milliseconds (3.064 GB/s)
* **C:** 141.04 milliseconds (7.0902 GB/s)
* **MRFStr:** 45.77 milliseconds (21.848 GB/s)

## mrfstr_repeat

```python
a = 'Hello' * (1024 * 1024 * 200) # 1GB
```

* **Python:** 331.43 milliseconds (3.017 GB/s)
* **C:** 126.82 milliseconds (7.885 GB/s)
* **MRFStr:** 48.90 milliseconds (20.450 GB/s)

## mrfstr_repeat_chr

```python
a = '0' * (1024 * 1024 * 1024) # 1GB
```

* **Python:** 166.52 milliseconds (6.005 GB/s)
* **C:** 28.84 milliseconds (34.674 GB/s)
* **MRFStr:** 23.04 milliseconds (43.403 GB/s)

## mrfstr_reverse

```python
a = '0' * (1024 * 1024 * 1024) # 1GB
a[::-1]
```

* **Python:** 593.23 milliseconds (1.686 GB/s)
* **C:** 632.79 milliseconds (1.580 GB/s)
* **MRFStr:** 47.46 milliseconds (21.070 GB/s)

## mrfstr_replace

```python
a = '0' * (1024 * 1024 * 1024) # 1GB
a.replace('0', '1')
```

* **Python:** 756.49 milliseconds (1.322 GB/s)
* **C:** 398.69 milliseconds (2.508 GB/s)
* **MRFStr:** 47.91 milliseconds (20.872 GB/s)

## mrfstr_equal

```python
a = '0' * (1024 * 1024 * 1024) # 1GB
b = '0' * (1024 * 1024 * 1024) # 1GB
a == b
```

* **Python:** 96.15 milliseconds (10.400 GB/s)
* **C:** 96.23 milliseconds (10.392 GB/s)
* **MRFStr:** 48.12 milliseconds (20.781 GB/s)

## mrfstr_contain

```python
a = '0' * (1024 * 1024 * 1024) # 1GB
'1' in a
```

* **Python:** 248.42 milliseconds (4.0254 GB/s)
* **C:** 252.75 milliseconds (3.956 GB/s)
* **MRFStr:** 24.56 milliseconds (40.717 GB/s)

## mrfstr_find

```python
a = '0' * (1024 * 1024 * 1024) # 1GB
a.find('1')
```

* **Python:** 248.64 milliseconds (4.0219 GB/s)
* **C:** 250.65 milliseconds (3.99 GB/s)
* **MRFStr:** 24.66 milliseconds (40.552 GB/s)
