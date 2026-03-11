# Hybrid Cryptography

**Carsten Baum & Tyge Tiessen**

---

## 1. Background

Due to the development of quantum computers, there currently is a process underway world-wide to upgrade cryptography that is insecure against quantum computers to cryptography that is quantum-secure.

Cryptography that is insecure against quantum computers is sometimes called **classical cryptography**.

Some cryptographic schemes that are conjectured to be secure against quantum computers have already been standardized, such as:

### Key Encapsulation Mechanisms (KEM)
- ML-KEM
- HQC

### Digital Signatures
- ML-DSA
- SLH-DSA

However, some of this cryptography relies on mathematical foundations that are **not as well researched** as established schemes based on:

- The hardness of factoring (RSA)
- The hardness of finding discrete logarithms in elliptic curve groups
  - EC-Diffie Hellman
  - EC-DSA

Because of this uncertainty, **ENISA and other organizations recommend the use of cryptography in a hybrid mode**.

Hybrid cryptography means that to achieve a cryptographic task, **a classical algorithm and a quantum-secure algorithm are used together**.

Security holds as long as **either of the algorithms remains secure**.

However, this also means that more cryptographic algorithms must be executed.

For example, when establishing a **TLS connection**, both:

- a classical key exchange
- a post-quantum key exchange

must be run **in parallel**.

This may:

- increase communication overhead
- require more computation from the machines running the TLS connection.

---

## 2. Tasks

In this project, you will investigate how hybrid cryptography is achieved in practice and what impact it has on performance.

The project consists of the following tasks.

---

### Task 1 – ETSI Hybrid Algorithms

Investigate the **ETSI recommendations on hybrid algorithms** for:

- digital signature schemes
- key encapsulation mechanisms

You must summarize in your report:

- which types of hybrid algorithms are mentioned in the document
- what their advantages and disadvantages are

Reference:

https://certification.enisa.europa.eu/document/download/a845662b-aee0-484e-9191-890c4cfa7aaa_en?filename=ECCG%20Agreed%20Cryptographic%20Mechanisms%20version%202.pdf

---

### Task 2 – ENISA Recommendations

Describe:

- which **quantum-secure (hybrid) algorithms** can be used within the EU
- which **parameter levels** are recommended

according to **ENISA’s recommendations**.

---

### Task 3 – TLS Implementation and Benchmarking

Use an existing software library (**OpenSSL**) to establish an authenticated connection via **TLS 1.3** between:

- a client program
- a server program

After establishing the TLS connection, send data via this authenticated connection.

You must test:

### Classical TLS

TLS 1.3 using **only classical cryptography**

### Hybrid TLS

TLS 1.3 using **hybrid Key Encapsulation Mechanisms (KEM)**

---

### Experiments

Implement and run experiments that measure:

1. The **amount of data sent** via the TLS connection
2. The **time required to establish the TLS connection**
3. The **time required to send data through the connection**

You must run your benchmarks **multiple times** in order to eliminate noise.

For the benchmark:

- choose **one classical algorithm combination**
- choose **one post-quantum algorithm combination**

You **do not need to benchmark all possible combinations**.

---

### Optional

If you want to simulate a more realistic network scenario, you may:

- limit bandwidth
- introduce network delay

This can be done using the Linux command `tc`.

---

## 3. Deliverables

Your submission must contain **two deliverables**.

---

### Deliverable 1 – Report

A report describing:

- how hybrid digital signatures function according to the **ETSI standard**
- which algorithms are recommended for use within the **EU**
- the **environment** used to test hybrid cryptography
- the **methodology** used in the experiments
- the **results** obtained

---

### Deliverable 2 – Code Submission

A `.zip` file containing:

- the code you wrote
- a `README` file
- the test cases

You must implement:

- a **client program**
- a **server program**

The programs must:

- connect via the **Linux loopback interface**
- use **OpenSSL version 3.5 or higher**
- securely establish the TLS connection

---

## 4. General Information

Both the **report** and the **implementation** are graded.

---

### Report Requirements

The report must be **self-contained**.

You must assume that the reader has **no familiarity with the specific project tasks**.

However, you may assume that the reader has knowledge similar to the **course prerequisites**.

The report must be:

- **maximum 12 pages**
- **excluding references and project contributions**

---

### Individual Contributions

Because grading must be done on an **individual basis**, you must indicate **individual contributions**.

### Report

Add a section at the end of the report with a table showing:

- which student worked on which sections of the report
- which student contributed to which parts of the project

This section **does not count toward the page limit**.

### Code

It is recommended to use **Git** to track individual contributions.

If you use Git, submit the **Git repository**.

---

### Assessment Criteria

The report and code will be evaluated based on:

- completion of the project tasks
- correctness of the proposed solutions
- appropriateness of the methods used
- quality of writing
- quality of implementation
- adherence to the project requirements

All **non-bonus tasks must be completed** to receive the highest grade.

To achieve a high grade:

- explain tasks clearly
- provide sufficient detail
- reflect on problems encountered
- describe possible solutions

The following will lower the grade:

- incorrect solutions
- inaccurate reports
- superficial explanations
- non-functioning code
- undocumented code
- poorly documented implementations

---

### Suggested Report Structure

The report should contain:

1. Introduction
2. Background and definitions
3. Description of the project tasks
4. Implementation details
5. Experimental results
6. Discussion

References must be included to justify claims and provide sources.

---

### Code Submission Requirements

Your code must:

- compile and run on **Ubuntu 24.04.2 LTS**

Follow standard **software engineering practices**:

- readable structure
- clear code organization
- proper documentation

Your repository must include a **README file** containing the following sections.

---

#### Compilation and Installation

Describe:

- prerequisites (libraries and versions)
- commands required to compile the code
- how the code should be installed

---

#### Running the Test Cases

Describe:

- commands required to run the tests
- how the outputs should be interpreted

---

### Running Benchmarks

If benchmarking is required, describe:

- commands required to run benchmarks
- how benchmark outputs should be interpreted

You must test the instructions in the README file before submission.

---

## AI Usage

Artificial intelligence tools may be used to **support learning**.

However, if AI tools are used to write:

- the report
- the code

this must be **clearly indicated**.

Failure to indicate AI usage will be considered **plagiarism**.

For more information on referencing AI usage, see:

https://www.bibliotek.dtu.dk/en/publishing/reference-management/kunstig-intelligens