import { createRoot } from 'react-dom/client'
import './styles/font.css'
import './styles/layout.css'
import { RouterProvider } from 'react-router';
import router from './router';

createRoot(document.getElementById('root')).render(
  <RouterProvider router={router} />
);