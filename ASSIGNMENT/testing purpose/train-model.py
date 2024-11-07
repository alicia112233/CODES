import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import accuracy_score, confusion_matrix
import joblib
import matplotlib.pyplot as plt
import seaborn as sns

# Load dataset
data = pd.read_csv("xsitemoves.csv")  # Adjust path as necessary
X = data.iloc[:, :-1]  # Input features
y = data['Outcome']  # Target variable: 1 for win, 0 for lose

# Split data
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Train Decision Tree model
model = DecisionTreeClassifier(max_depth=3)  # Adjust max_depth for complexity
model.fit(X_train, y_train)

# Calculate accuracy and confusion matrix
train_accuracy = accuracy_score(y_train, model.predict(X_train))
test_accuracy = accuracy_score(y_test, model.predict(X_test))
conf_matrix = confusion_matrix(y_test, model.predict(X_test))

print(f"Training Accuracy: {train_accuracy * 100:.2f}%")
print(f"Testing Accuracy: {test_accuracy * 100:.2f}%")

# Plot confusion matrix
sns.heatmap(conf_matrix, annot=True, fmt="d", cmap="Blues", xticklabels=['Lose', 'Win'], yticklabels=['Lose', 'Win'])
plt.xlabel("Predicted")
plt.ylabel("Actual")
plt.title("Confusion Matrix")
plt.show()

# Save model
joblib.dump(model, 'tic_tac_toe_ai_model.pkl')