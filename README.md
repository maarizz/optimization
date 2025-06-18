# Axis-Aligned Enclosure Optimization

## 🏙️ Problem Overview

As the chief architect of **Axion Prime**, a futuristic, grid-structured mega-city, you are tasked with urban redevelopment by defining containment boundaries that minimize cost. The city has numerous buildings (each with an associated weight/cost), and your objective is to enclose **at least K buildings** within a **simple, axis-aligned polygon** (edges parallel to the x and y axes), minimizing the total cost.

> **Total Cost = Perimeter of the Polygon + Sum of Weights of Enclosed Buildings**

---

## 🎯 Goals

- Design an **axis-aligned**, **simple**, **closed polygon**.
- Enclose **at least K buildings**.
- Minimize the **total cost** as defined above.

### ✅ Conditions for a Valid Polygon

- Must be **axis-aligned** (edges parallel to x/y axes).
- May include **floating-point vertices**.
- Must be **simple** (non-self-intersecting and closed).
- A building is **enclosed** if it lies **strictly inside or on the boundary** of the polygon.

---

## 🧾 Input Format



- `N`: Total number of buildings.
- `K`: Minimum number of buildings to enclose.
- Each building is defined by coordinates `(x, y)` and weight `w`.

---

## 🔒 Constraints

- `1 ≤ K ≤ N ≤ 10000`
- `0 ≤ xᵢ, yᵢ ≤ 10000`
- `wᵢ` can be **negative**
- Polygon must be **simple**, **closed**, and **axis-aligned**

---

## 📤 Output Format

- First line: A **floating-point number C**, the **minimum total cost**, with **at least six digits after the decimal point**.
- Next lines: One **edge per line** describing the polygon.

Each edge should be:

Where:
- Each edge connects point `(x₁, y₁)` to `(x₂, y₂)`
- All edges must form a **closed loop**, listed **in order**
- Edges must be **either horizontal or vertical**

---

## 💡 Sample Usage

You can compile and run the implementation with:

```bash
g++ -std=c++17 -O2 opti.cpp -o opti
./opti < input.txt > output.txt
